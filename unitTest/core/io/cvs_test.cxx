#include "../../TEST.h"
#include <dataframe/io/csv.h>
#include <fstream>
#include <sstream>

TEST(IO, CSV) {
    // Create a test DataFrame
    df::Dataframe test_df;
    test_df.add("integers", df::Serie<int64_t>({1, 2, 3, 4, 5}));
    test_df.add("doubles", df::Serie<double>({1.1, 2.2, 3.3, 4.4, 5.5}));
    test_df.add("strings",
                df::Serie<std::string>({"a", "b,c", "d\"e", "f", "g"}));

    // Write to CSV
    df::io::write_csv(test_df, "test.csv");

    // Read back
    auto read_df = df::io::read_csv("test.csv");

    // Verify structure
    EXPECT_EQ(read_df.size(), 3);
    EXPECT_EQ(read_df.get<int64_t>("integers").size(), 5);
    EXPECT_EQ(read_df.get<double>("doubles").size(), 5);
    EXPECT_EQ(read_df.get<std::string>("strings").size(), 5);

    // Verify content
    EXPECT_EQ(read_df.get<int64_t>("integers")[2], 3);
    EXPECT_NEAR(read_df.get<double>("doubles")[1], 2.2, 1e-10);
    EXPECT_STREQ(read_df.get<std::string>("strings")[2].c_str(), "d\"e");
}

TEST(IO, CSV_Options) {
    // Test CSV with different options
    std::string csv_content = R"(
col1;col2;col3
1;1.1;text1
2;2.2;text2
)";

    std::ofstream test_file("test_semicolon.csv");
    test_file << csv_content;
    test_file.close();

    df::io::CSVOptions options;
    options.delimiter = ';';
    options.skip_rows = 1; // Skip the empty first line

    auto df = df::io::read_csv("test_semicolon.csv", options);
    EXPECT_EQ(df.size(), 3);
    EXPECT_EQ(df.get<int64_t>("col1").size(), 2);
}

TEST(IO, CSV_Edge_Cases) {
    // Test edge cases like empty values, quoted strings with delimiters
    std::string csv_content = R"(
col1,col2,col3
1,,text with, comma
2,"quoted, text",text
3,3.3,"text with ""quotes"""
)";

    std::ofstream test_file("edge_cases.csv");
    test_file << csv_content;
    test_file.close();

    auto df = df::io::read_csv("edge_cases.csv");
    EXPECT_EQ(df.size(), 3);
    EXPECT_EQ(df.get<int64_t>("col1").size(), 3);
    EXPECT_STREQ(df.get<std::string>("col2")[1].c_str(), "quoted, text");
    EXPECT_STREQ(df.get<std::string>("col3")[2].c_str(),
                 "text with \"quotes\"");
}

RUN_TESTS()