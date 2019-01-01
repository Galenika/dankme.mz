#pragma once

#include <map>
#include <Windows.h>
#include <cstdint>
#include <stdexcept>
#include <assert.h>

typedef DWORD** PPDWORD;

class VFTableHook {
	VFTableHook(const VFTableHook&) = delete;
public:

	template<class Type>
	static Type HookManual(uintptr_t *instance, int offset, Type hook)
	{
		DWORD Dummy;
		Type fnOld = (Type)instance[offset];
		VirtualProtect((void*)(instance + offset * 0x4), 0x4, PAGE_EXECUTE_READWRITE, &Dummy);
		instance[offset] = (uintptr_t)hook;
		VirtualProtect((void*)(instance + offset * 0x4), 0x4, Dummy, &Dummy);
		return fnOld;
	}

private:

	static int Unprotect(void *region)
	{
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery((LPCVOID)region, &mbi, sizeof(mbi));
		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, PAGE_EXECUTE_READWRITE, &mbi.Protect);
		return mbi.Protect;
	}

	static void Protect(void *region, int protection)
	{
		MEMORY_BASIC_INFORMATION mbi;
		VirtualQuery((LPCVOID)region, &mbi, sizeof(mbi));
		VirtualProtect(mbi.BaseAddress, mbi.RegionSize, protection, &mbi.Protect);
	}
};

class ProtectGuard
{
public:

	ProtectGuard(void *base, uint32_t len, uint32_t protect)
	{
		this->base = base;
		this->len = len;

		if (!VirtualProtect(base, len, protect, (PDWORD)&old_protect))
			throw std::runtime_error("Failed to protect region!");
	}

	~ProtectGuard()
	{
		VirtualProtect(base, len, old_protect, (PDWORD)&old_protect);
	}

private:

	void *base;
	uint32_t len;
	uint32_t old_protect;
};

class ShadowVTManager
{

public:

	ShadowVTManager() : class_base(nullptr), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	ShadowVTManager(void *base) : class_base(base), method_count(0), shadow_vtable(nullptr), original_vtable(nullptr) {}
	~ShadowVTManager()
	{
		RestoreTable();

		delete[] shadow_vtable;
	}

	inline void Setup(void *base = nullptr)
	{
		if (base != nullptr)
			class_base = base;

		if (class_base == nullptr)
			return;

		original_vtable = *(uintptr_t**)class_base;
		method_count = GetMethodCount(original_vtable);

		if (method_count == 0)
			return;

		shadow_vtable = new uintptr_t[method_count + 1]();

		shadow_vtable[0] = original_vtable[-1];
		std::memcpy(&shadow_vtable[1], original_vtable, method_count * sizeof(uintptr_t));

		try
		{
			auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
			*(uintptr_t**)class_base = &shadow_vtable[1];
		}
		catch (...)
		{
			delete[] shadow_vtable;
		}
	}

	template<typename T>
	inline void Hook(uint32_t index, T method)
	{
		assert(index < method_count);
		shadow_vtable[index + 1] = reinterpret_cast<uintptr_t>(method);
	}

	inline void Unhook(uint32_t index)
	{
		assert(index < method_count);
		shadow_vtable[index + 1] = original_vtable[index];
	}

	template<typename T>
	inline T GetOriginal(uint32_t index)
	{
		return (T)original_vtable[index];
	}

	inline void RestoreTable()
	{
		try
		{
			if (original_vtable != nullptr)
			{
				auto guard = ProtectGuard{ class_base, sizeof(uintptr_t), PAGE_READWRITE };
				*(uintptr_t**)class_base = original_vtable;
				original_vtable = nullptr;
			}
		}
		catch (...) {}
	}

private:

	inline uint32_t GetMethodCount(uintptr_t *vtable_start)
	{
		uint32_t len = -1;

		do ++len; while (vtable_start[len]);

		return len;
	}

	void *class_base;
	uint32_t method_count;
	uintptr_t *shadow_vtable;
	uintptr_t *original_vtable;
};

class UDVMT
{
public:
	uintptr_t * search_free_data_page(const char * module_name, const std::size_t vmt_size);
	UDVMT();
	UDVMT(void* base);
	~UDVMT();

	bool setup(void * base, const char * moduleName = nullptr);
	template<typename T>
	void hook_index(int index, T fun)
	{
		assert(index >= 0 && index <= (int)vftbl_len);
		new_vftb1[index + 1] = reinterpret_cast<std::uintptr_t>(fun);
	}
	void unhook_index(int index)
	{
		new_vftb1[index] = old_vftbl[index];
	}
	void unhook_all()
	{
		try {
			if (old_vftbl != nullptr) {
				auto guard = ProtectGuard{ class_base, sizeof(std::uintptr_t), PAGE_READWRITE };
				*(std::uintptr_t**)class_base = old_vftbl;
				old_vftbl = nullptr;
			}
		}
		catch (...) { }
	}

	template<typename T>
	T get_original(int index)
	{
		return (T)old_vftbl[index];
	}

private:

	static inline std::size_t estimate_vftbl_length(std::uintptr_t* vftbl_start);

	void*           class_base;
	std::size_t     vftbl_len;
	std::uintptr_t* new_vftb1;
	std::uintptr_t* old_vftbl;
	LPCVOID         search_base = nullptr;
	bool wasAllocated = false;
};