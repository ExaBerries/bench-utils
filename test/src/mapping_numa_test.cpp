#include <gtest/gtest.h>
#include "mapping.h"

TEST(MappingNuma, i7700k) {
	using namespace bench_utils;
	auto tree = create_7700k_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 8u);

	// actual cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 2u);
	EXPECT_EQ(mapping[2].os_id, 4u);
	EXPECT_EQ(mapping[3].os_id, 6u);

	// hyperthreads
	EXPECT_EQ(mapping[4].os_id, 1u);
	EXPECT_EQ(mapping[5].os_id, 3u);
	EXPECT_EQ(mapping[6].os_id, 5u);
	EXPECT_EQ(mapping[7].os_id, 7u);
}

TEST(MappingNuma, DualSocket2c2t) {
	using namespace bench_utils;
	auto tree = create_2s2c2t_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 8u);
	
	// actual cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 4u);
	EXPECT_EQ(mapping[2].os_id, 2u);
	EXPECT_EQ(mapping[3].os_id, 6u);

	// hyperthreads
	EXPECT_EQ(mapping[4].os_id, 1u);
	EXPECT_EQ(mapping[5].os_id, 5u);
	EXPECT_EQ(mapping[6].os_id, 3u);
	EXPECT_EQ(mapping[7].os_id, 7u);
}

TEST(MappingNuma, Alder2p2e) {
	using namespace bench_utils;
	auto tree = create_alder2p2e_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 6u);
	
	// actual cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 2u);
	EXPECT_EQ(mapping[2].os_id, 4u);
	EXPECT_EQ(mapping[3].os_id, 5u);

	// hyperthreads
	EXPECT_EQ(mapping[4].os_id, 1u);
	EXPECT_EQ(mapping[5].os_id, 3u);
}

TEST(MappingNuma, Meteor2p2e2lpe) {
	using namespace bench_utils;
	auto tree = create_meteor2p2e2lpe_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 8u);
	
	// p cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 2u);

	// e+lpe cores
	EXPECT_EQ(mapping[2].os_id, 4u);
	EXPECT_EQ(mapping[3].os_id, 6u);
	EXPECT_EQ(mapping[4].os_id, 5u);
	EXPECT_EQ(mapping[5].os_id, 7u);

	// hyperthreads
	EXPECT_EQ(mapping[6].os_id, 1u);
	EXPECT_EQ(mapping[7].os_id, 3u);
}

TEST(MappingNuma, MultiCCD) {
	using namespace bench_utils;
	auto tree = create_2ccd_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 16u);

	// cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 8u);
	EXPECT_EQ(mapping[2].os_id, 2u);
	EXPECT_EQ(mapping[3].os_id, 10u);
	EXPECT_EQ(mapping[4].os_id, 4u);
	EXPECT_EQ(mapping[5].os_id, 12u);
	EXPECT_EQ(mapping[6].os_id, 6u);
	EXPECT_EQ(mapping[7].os_id, 14u);

	// hyperthreads
	EXPECT_EQ(mapping[8].os_id, 1u);
	EXPECT_EQ(mapping[9].os_id, 9u);
	EXPECT_EQ(mapping[10].os_id, 3u);
	EXPECT_EQ(mapping[11].os_id, 11u);
	EXPECT_EQ(mapping[12].os_id, 5u);
	EXPECT_EQ(mapping[13].os_id, 13u);
	EXPECT_EQ(mapping[14].os_id, 7u);
	EXPECT_EQ(mapping[15].os_id, 15u);
}

TEST(MappingNuma, DualSocketMultiCCD) {
	using namespace bench_utils;
	auto tree = create_2s2ccd_tree();
	auto mapping = create_mapping_numa(tree);

	ASSERT_EQ(mapping.size(), 32u);

	// cores
	EXPECT_EQ(mapping[0].os_id, 0u);
	EXPECT_EQ(mapping[1].os_id, 16u);
	EXPECT_EQ(mapping[2].os_id, 8u);
	EXPECT_EQ(mapping[3].os_id, 24u);

	EXPECT_EQ(mapping[4].os_id, 2u);
	EXPECT_EQ(mapping[5].os_id, 18u);
	EXPECT_EQ(mapping[6].os_id, 10u);
	EXPECT_EQ(mapping[7].os_id, 26u);

	EXPECT_EQ(mapping[8].os_id, 4u);
	EXPECT_EQ(mapping[9].os_id, 20u);
	EXPECT_EQ(mapping[10].os_id, 12u);
	EXPECT_EQ(mapping[11].os_id, 28u);

	EXPECT_EQ(mapping[12].os_id, 6u);
	EXPECT_EQ(mapping[13].os_id, 22u);
	EXPECT_EQ(mapping[14].os_id, 14u);
	EXPECT_EQ(mapping[15].os_id, 30u);

	// hyperthreads
	EXPECT_EQ(mapping[16].os_id, 1u);
	EXPECT_EQ(mapping[17].os_id, 17u);
	EXPECT_EQ(mapping[18].os_id, 9u);
	EXPECT_EQ(mapping[19].os_id, 25u);

	EXPECT_EQ(mapping[20].os_id, 3u);
	EXPECT_EQ(mapping[21].os_id, 19u);
	EXPECT_EQ(mapping[22].os_id, 11u);
	EXPECT_EQ(mapping[23].os_id, 27u);

	EXPECT_EQ(mapping[24].os_id, 5u);
	EXPECT_EQ(mapping[25].os_id, 21u);
	EXPECT_EQ(mapping[26].os_id, 13u);
	EXPECT_EQ(mapping[27].os_id, 29u);

	EXPECT_EQ(mapping[28].os_id, 7u);
	EXPECT_EQ(mapping[29].os_id, 23u);
	EXPECT_EQ(mapping[30].os_id, 15u);
	EXPECT_EQ(mapping[31].os_id, 31u);
}
