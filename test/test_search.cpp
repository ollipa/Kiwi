#include "gtest/gtest.h"
#include <algorithm>

#include <kiwi/ArchUtils.h>
#include "../src/search.h"

using namespace kiwi;

template<ArchType arch>
void testFindAll()
{
	static constexpr size_t numBits = sizeof(size_t) * 8;
	// SIMD versions may read up to 64 bytes
	uint8_t arr[64];

	std::fill(arr, arr + 64, 7);
	for (size_t size = 1; size <= numBits; ++size)
	{
		const size_t expected = size == numBits ? ~(size_t)0 : ((size_t)1 << size) - 1;
		EXPECT_EQ(nst::findAll<arch>(arr, size, 7), expected) << archToStr(arch) << " size=" << size;
	}

	for (size_t i = 0; i < 64; ++i) arr[i] = i % 3 == 0 ? 7 : 8;
	for (size_t size = 1; size <= numBits; ++size)
	{
		size_t expected = 0;
		for (size_t i = 0; i < size; i += 3) expected |= (size_t)1 << i;
		EXPECT_EQ(nst::findAll<arch>(arr, size, 7), expected) << archToStr(arch) << " size=" << size;
	}
}

TEST(Search, FindAll)
{
	testFindAll<ArchType::none>();
	testFindAll<ArchType::balanced>();
#if defined(__x86_64__) || defined(KIWI_ARCH_X86) || defined(KIWI_ARCH_X86_64)
	testFindAll<ArchType::sse2>();
#endif
#ifdef KIWI_ARCH_ARM64
	testFindAll<ArchType::neon>();
#endif
}
