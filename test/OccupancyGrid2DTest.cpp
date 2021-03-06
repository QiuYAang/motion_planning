#include "gtest/gtest.h"
#include <motion_planning/State/Pose2D.hpp>
#include <motion_planning/Occupancy/OccupancyGrid2D.hpp>

class OccupancyGrid2DTest : public ::testing::Test {
    public:
        static constexpr const char* tiny = "maps/tiny.png";
        static constexpr const char* classroom_incomplete = "maps/classroom_incomplete.png";
};

TEST_F(OccupancyGrid2DTest, CreateMap) {
    OccupancyGrid2D<Pose2D> occ;
}

TEST_F(OccupancyGrid2DTest, TestOccupancyProbability) {
    OccupancyGrid2D<Pose2D> occ;

    double resolution = 1.;
    Pose2D origin = {.x=0, .y=0, .theta=0};
    occ.setMap(tiny, resolution, origin);
    occ.setObjectRadius(0.44, 0.01);

    Pose2D query = {.x=0, .y=0, .theta=0};
    ASSERT_EQ(0, occ.occupancyProbability(&query));
    query.x = 1;
    ASSERT_EQ(1, occ.occupancyProbability(&query));
    query.x = 2;
    ASSERT_NEAR(0.5, occ.occupancyProbability(&query), 0.01);
    query.y = 1;
    ASSERT_EQ(0, occ.occupancyProbability(&query));
    query.x = 1;
    ASSERT_NEAR(0.5, occ.occupancyProbability(&query), 0.01);
    query.x = 0;
    ASSERT_NEAR(0.75, occ.occupancyProbability(&query), 0.01);
    query.y = 2;
    ASSERT_EQ(1, occ.occupancyProbability(&query));
    query.x = 1;
    ASSERT_NEAR(0.25, occ.occupancyProbability(&query), 0.01);
    query.x = 2;
    ASSERT_EQ(1, occ.occupancyProbability(&query));
}

TEST_F(OccupancyGrid2DTest, SampleFreeRotated) {
    OccupancyGrid2D<Pose2D> occ;

    double resolution = 0.05;
    int width = 2./resolution;
    int height = 2./resolution;
    std::vector<int8_t> data(width*height, 0);
    Pose2D origin = {.x=-20, .y=4, .theta=-1.1};
    occ.setMap(data, width, height, resolution, origin);
    occ.setObjectRadius(0.44, 0.01);

    Pose2D state;
    for (int i = 0; i < 100; i++) {
        occ.randomState(&state);
        ASSERT_TRUE(occ.isFree(&state));
        ASSERT_FALSE(occ.isUnknown(&state));
        ASSERT_FALSE(occ.isOccupied(&state));
    }
}

TEST_F(OccupancyGrid2DTest, InitializeWithBytes) {
    OccupancyGrid2D<Pose2D> occ;
    std::vector<uint8_t> data = {0, 255, 0, 127, 63, 191};

    double resolution = 1.;
    Pose2D origin = {.x=0, .y=0, .theta=0};
    occ.setMap(data, 3, 2, resolution, origin);
    occ.setObjectRadius(0.44, 0.01);

    Pose2D query = {.x=0, .y=0, .theta=0};
    ASSERT_EQ(1, occ.occupancyProbability(&query)); // 0
    query.x = 1;
    ASSERT_EQ(0, occ.occupancyProbability(&query)); // 255
    query.x = 2;
    ASSERT_EQ(1, occ.occupancyProbability(&query)); // 0

    query.x = 0;
    query.y = 1;
    ASSERT_NEAR(0.5, occ.occupancyProbability(&query), 0.01); // 127
    query.x = 1;
    ASSERT_NEAR(0.75, occ.occupancyProbability(&query), 0.01); // 63
    query.x = 2;
    ASSERT_NEAR(0.25, occ.occupancyProbability(&query), 0.01); // 191
}

TEST_F(OccupancyGrid2DTest, InitializeWithInts) {
    OccupancyGrid2D<Pose2D> occ;
    std::vector<int8_t> data = {-1, 42, 100, 0, 50, 69};

    double resolution = 1.;
    Pose2D origin = {.x=0, .y=0, .theta=0};
    occ.setMap(data, 2, 3, resolution, origin);
    occ.setObjectRadius(0.44, 0.01);

    Pose2D query = {.x=0, .y=0, .theta=0};
    ASSERT_EQ(0.5, occ.occupancyProbability(&query)); // -1
    query.x = 1;
    ASSERT_EQ(0.42, occ.occupancyProbability(&query)); // 42

    query.y = 1;
    query.x = 0;
    ASSERT_EQ(1, occ.occupancyProbability(&query)); // 100
    query.x = 1;
    ASSERT_EQ(0, occ.occupancyProbability(&query)); // 0

    query.y = 2;
    query.x = 0;
    ASSERT_EQ(0.5, occ.occupancyProbability(&query)); // 50
    query.x = 1;
    ASSERT_EQ(0.69, occ.occupancyProbability(&query)); // 68
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
