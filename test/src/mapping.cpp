#include "mapping.h"

namespace bench_utils {
	// struct logical_processor {
	// 	uint32_t thread_index = 0u;
	// 	uint32_t core_id = 0u; // to easily find the actual core without making a LUT
	// 	uint32_t os_id = 0u; // os specific core id
	// 	uint16_t group_id = 0u; // for thread groups in windows
	// };

	[[nodiscard]] topology_tree create_7700k_tree() noexcept {
		return {
			{{0u, {// numa
				{0u, { // llc
					{
						0u, // id
						0u, // perf_level
						{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
					},
					{
						1u, // id
						0u, // perf_level
						{{0u, 1u, 2u, 0u}, {1u, 1u, 3u, 0u}} // logical
					},
					{
						2u, // id
						0u, // perf_level
						{{0u, 2u, 4u, 0u}, {1u, 2u, 5u, 0u}} // logical
					},
					{
						3u, // id
						0u, // perf_level
						{{0u, 3u, 6u, 0u}, {1u, 3u, 7u, 0u}} // logical
					},
				}}
			}}},
			1u // perf_level_count
		};
	}

	[[nodiscard]] topology_tree create_2s2c2t_tree() noexcept {
		return {
			{
				{0u, { // numa
					{0u, { // llc
						{
							0u, // id
							0u, // perf_level
							{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
						},
						{
							1u, // id
							0u, // perf_level
							{{0u, 1u, 2u, 0u}, {1u, 1u, 3u, 0u}} // logical
						},
					}}
				}},
				{1u, { // numa
					{1u, { // llc
						{
							2u, // id
							0u, // perf_level
							{{0u, 2u, 4u, 0u}, {1u, 2u, 5u, 0u}} // logical
						},
						{
							3u, // id
							0u, // perf_level
							{{0u, 3u, 6u, 0u}, {1u, 3u, 7u, 0u}} // logical
						},
					}}
				}}
			},
			1u // perf_level_count
		};
	}

	[[nodiscard]] topology_tree create_alder2p2e_tree() noexcept {
		return {
			{{0u, { // numa
				{0u, { // llc
					{
						0u, // id
						0u, // perf_level
						{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
					},
					{
						1u, // id
						0u, // perf_level
						{{0u, 1u, 2u, 0u}, {1u, 1u, 3u, 0u}} // logical
					},
					{
						2u, // id
						1u, // perf_level
						{{0u, 2u, 4u, 0u}} // logical
					},
					{
						3u, // id
						1u, // perf_level
						{{0u, 3u, 5u, 0u}} // logical
					},
				}
			}}}},
			2u // perf_level_count
		};
	}

	[[nodiscard]] topology_tree create_meteor2p2e2lpe_tree() noexcept {
		return {
			{
				{0u, { // numa
					{0u, { // llc
						{
							0u, // id
							0u, // perf_level
							{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
						},
						{
							1u, // id
							0u, // perf_level
							{{0u, 0u, 2u, 0u}, {1u, 0u, 3u, 0u}} // logical
						},
						{
							2u, // id
							1u, // perf_level
							{{0u, 2u, 4u, 0u},} // logical
						},
						{
							3u, // id
							1u, // perf_level
							{{0u, 3u, 5u, 0u},} // logical
						},
					}},
				}},
				{1u, { // numa
					{1u, { // llc
						{
							4u, // id
							1u, // perf_level
							{{0u, 4u, 6u, 0u},} // logical
						},
						{
							5u, // id
							1u, // perf_level
							{{0u, 5u, 7u, 0u},} // logical
						},
					}}
				}}
			},
			2u // perf_level_count
		};
	}

	[[nodiscard]] topology_tree create_2ccd_tree() noexcept {
		return {
			{{0u, { // numa
				{0u, { // llc
					{
						0u, // id
						0u, // perf_level
						{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
					},
					{
						1u, // id
						0u, // perf_level
						{{0u, 1u, 2u, 0u}, {1u, 1u, 3u, 0u}} // logical
					},
					{
						2u, // id
						0u, // perf_level
						{{0u, 2u, 4u, 0u}, {1u, 2u, 5u, 0u}} // logical
					},
					{
						3u, // id
						0u, // perf_level
						{{0u, 3u, 6u, 0u}, {1u, 3u, 7u, 0u}} // logical
					},
				}},
				{1u, { // llc
					{
						4u, // id
						0u, // perf_level
						{{0u, 4u, 8u, 0u}, {1u, 4u, 9u, 0u}} // logical
					},
					{
						5u, // id
						0u, // perf_level
						{{0u, 5u, 10u, 0u}, {1u, 5u, 11u, 0u}} // logical
					},
					{
						6u, // id
						0u, // perf_level
						{{0u, 6u, 12u, 0u}, {1u, 6u, 13u, 0u}} // logical
					},
					{
						7u, // id
						0u, // perf_level
						{{0u, 7u, 14u, 0u}, {1u, 7u, 15u, 0u}} // logical
					},
				}
			}}}},
			1u // perf_level_count
		};
	}

	[[nodiscard]] topology_tree create_2s2ccd_tree() noexcept {
		return {
			{
				{0u, { // numa
					{0u, { // llc
						{
							0u, // id
							0u, // perf_level
							{{0u, 0u, 0u, 0u}, {1u, 0u, 1u, 0u}} // logical
						},
						{
							1u, // id
							0u, // perf_level
							{{0u, 1u, 2u, 0u}, {1u, 1u, 3u, 0u}} // logical
						},
						{
							2u, // id
							0u, // perf_level
							{{0u, 2u, 4u, 0u}, {1u, 2u, 5u, 0u}} // logical
						},
						{
							3u, // id
							0u, // perf_level
							{{0u, 3u, 6u, 0u}, {1u, 3u, 7u, 0u}} // logical
						},
					}},
					{1u, { // llc
						{
							4u, // id
							0u, // perf_level
							{{0u, 4u, 8u, 0u}, {1u, 4u, 9u, 0u}} // logical
						},
						{
							5u, // id
							0u, // perf_level
							{{0u, 5u, 10u, 0u}, {1u, 5u, 11u, 0u}} // logical
						},
						{
							6u, // id
							0u, // perf_level
							{{0u, 6u, 12u, 0u}, {1u, 6u, 13u, 0u}} // logical
						},
						{
							7u, // id
							0u, // perf_level
							{{0u, 7u, 14u, 0u}, {1u, 7u, 15u, 0u}} // logical
						},
					}
				}}},
				{1u, { // numa
					{2u, { // llc
						{
							8u, // id
							0u, // perf_level
							{{0u, 8u, 16u, 0u}, {1u, 8u, 17u, 0u}} // logical
						},
						{
							9u, // id
							0u, // perf_level
							{{0u, 9u, 18u, 0u}, {1u, 9u, 19u, 0u}} // logical
						},
						{
							10u, // id
							0u, // perf_level
							{{0u, 10u, 20u, 0u}, {1u, 10u, 21u, 0u}} // logical
						},
						{
							11u, // id
							0u, // perf_level
							{{0u, 11u, 22u, 0u}, {1u, 11u, 23u, 0u}} // logical
						},
					}},
					{3u, { // llc
						{
							12u, // id
							0u, // perf_level
							{{0u, 12u, 24u, 0u}, {1u, 12u, 25u, 0u}} // logical
						},
						{
							13u, // id
							0u, // perf_level
							{{0u, 13u, 26u, 0u}, {1u, 13u, 27u, 0u}} // logical
						},
						{
							14u, // id
							0u, // perf_level
							{{0u, 14u, 28u, 0u}, {1u, 14u, 29u, 0u}} // logical
						},
						{
							15u, // id
							0u, // perf_level
							{{0u, 15u, 30u, 0u}, {1u, 15u, 31u, 0u}} // logical
						},
					}
				}}}
			},
			1u // perf_level_count
		};
	}
} // namespace bench_utils
