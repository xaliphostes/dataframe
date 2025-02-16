#include "../../TEST.h"
#include <dataframe/io/json.h>
#include <fstream>
#include <sstream>

TEST(IO, JSON) {
    // Create a test DataFrame
    df::Dataframe test_df;
    test_df.add("integers", df::Serie<int64_t>({1, 2, 3}));
    test_df.add("doubles", df::Serie<double>({1.1, 2.2, 3.3}));
    test_df.add("strings", df::Serie<std::string>({"a", "b", "c"}));

    // Write to JSON
    df::io::write_json(test_df, "test.json", true);

    // Read back
    auto read_df = df::io::read_json("test.json");

    // Verify structure
    EXPECT_EQ(read_df.size(), 3);
    EXPECT_EQ(read_df.get<int64_t>("integers").size(), 3);
    EXPECT_EQ(read_df.get<double>("doubles").size(), 3);
    EXPECT_EQ(read_df.get<std::string>("strings").size(), 3);

    // Verify content
    EXPECT_EQ(read_df.get<int64_t>("integers")[1], 2);
    EXPECT_NEAR(read_df.get<double>("doubles")[2], 3.3, 1e-10);
    EXPECT_STREQ(read_df.get<std::string>("strings")[0].c_str(), "a");
}

TEST(IO, JSON_Empty) {
    df::Dataframe empty_df;
    df::io::write_json(empty_df, "empty.json");
    auto read_df = df::io::read_json("empty.json");
    EXPECT_EQ(read_df.size(), 0);
}

RUN_TESTS()