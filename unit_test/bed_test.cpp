#include <boost/filesystem.hpp>
#include <Biovoltron/format/bed.hpp>
#include <Nucleona/app/cli/gtest.hpp>
#include <Nucleona/test/data_dir.hpp>
#include <Nucleona/sys/executable_dir.hpp>
using namespace biovoltron::format;

// header
std::string file_colorByStrand() {         
    return (nucleona::test::data_dir() / "bed" / "header" / "colorByStrand.txt").string();
}

std::string file_itemRGB() {         
    return (nucleona::test::data_dir() / "bed" / "header" / "itemRGB.txt").string();
}

std::string file_useScore() {         
    return (nucleona::test::data_dir() / "bed" / "header" / "useScore.txt").string();
}

// only BED data
std::string file_column_not_consistent_bed() {  // error, num of column must be consistent
    return (nucleona::test::data_dir() / "bed" / "column_not_consisten.bed").string();
}

std::string file_BED1() {           // error, must have at least 3 columns
    return (nucleona::test::data_dir() / "bed" / "1_column.bed").string();
}

std::string file_not_bed() {         // error
    return (nucleona::test::data_dir() / "bed" / "not_bed.txt").string();
}

std::string file_BED3() {         
    return (nucleona::test::data_dir() / "bed" / "3_column.bed").string();
}

std::string file_BED12() {
    return (nucleona::test::data_dir() / "bed" / "12_column.bed").string();
}

// complete BED
std::string file_BED3_with_header() {         
    return (nucleona::test::data_dir() / "bed" / "3_column_with_header.bed").string();
}



TEST (Header, set)
{
    std::ifstream ifs(file_colorByStrand());
    bed::Header h;
    std::string str;
    std::getline(ifs, str);

    h.set(str);

    std::vector<uint32_t> forward = { 255,0,0 }, reverse = { 0,0,255 };


    EXPECT_EQ(str, h.to_string());
    EXPECT_EQ("ColorByStrandDemo", h.name);
    EXPECT_EQ("Color by strand demonstration", h.description);
    EXPECT_EQ(2, h.visibility);
    EXPECT_TRUE (h.color_by_strand);
    EXPECT_EQ(forward[0], h.forward_strand_color[0]);
    EXPECT_EQ(forward[1], h.forward_strand_color[1]);
    EXPECT_EQ(forward[2], h.forward_strand_color[2]);
    EXPECT_EQ(reverse[0], h.reverse_strand_color[0]);
    EXPECT_EQ(reverse[1], h.reverse_strand_color[1]);
    EXPECT_EQ(reverse[2], h.reverse_strand_color[2]);
}

TEST (Header, set2)
{
    std::string str("track name=\"CHIP_Jaridz_1216G_KD_R2_S9_R1_001_TF_peaks_bed\" description=\"CHIP_Jaridz_1216G_KD_R2_S9_R1_001_TF_peaks_bed\" useScore=0");
    bed::Header h;

    h.set(str);

    EXPECT_EQ(str, h.to_string());
    EXPECT_EQ("CHIP_Jaridz_1216G_KD_R2_S9_R1_001_TF_peaks_bed", h.name);
    EXPECT_EQ("CHIP_Jaridz_1216G_KD_R2_S9_R1_001_TF_peaks_bed", h.description);
    EXPECT_FALSE(h.use_score);
}

TEST (BED, BED3)
{
    std::ifstream ifs(file_BED3());
    BED< std::tuple <std::string, uint32_t, uint32_t>> bed;

    std::string str;
    std::getline(ifs, str);
    bed.set_bed_data(str);
    

    auto& [ chrom, chrom_start, chrom_end ] = bed.data;
    
    EXPECT_EQ("chr1", chrom);
    EXPECT_EQ(85000835, chrom_start);
    EXPECT_EQ(85003645, chrom_end);
    EXPECT_EQ(str, bed.to_string());
}

TEST (BED, BED12_vector)
{
    using TupleType = std::tuple <std::string, uint32_t, uint32_t, std::string, uint16_t, char, uint32_t, uint32_t,
                                 uint32_t, uint16_t, std::vector<uint32_t>, std::vector<uint32_t> >;

    
    BED<TupleType> bed;
    std::string str = "chr1\t85000835\t85003645\tuc001aaa\t0\t+\t11873\t11873\t0\t3\t354,109,1189,\t0,739,1347,";
    bed.set_bed_data(str);

    std::vector<uint32_t> expected_block_sizes =  { 354, 109, 1189 };
    std::vector<uint32_t> expected_block_starts = {   0, 739, 1347 };

    
    
    EXPECT_EQ("chr1",               std::get<bed::Col::chrom>(bed.data));
    EXPECT_EQ(85000835,             std::get<bed::Col::chromStart>(bed.data));
    EXPECT_EQ(85003645,             std::get<bed::Col::chromEnd>(bed.data));
    EXPECT_EQ("uc001aaa",           std::get<bed::Col::name>(bed.data));
    EXPECT_EQ(0,                    std::get<bed::Col::score>(bed.data));
    EXPECT_EQ('+',                  std::get<bed::Col::strand>(bed.data));
    EXPECT_EQ(11873,                std::get<bed::Col::thickStart>(bed.data));
    EXPECT_EQ(11873,                std::get<bed::Col::thickEnd>(bed.data));
    EXPECT_EQ(0,                    std::get<bed::Col::itemRgb>(bed.data));
    EXPECT_EQ(3,                    std::get<bed::Col::blockCount>(bed.data));
    EXPECT_EQ(expected_block_sizes, std::get<bed::Col::blockSizes>(bed.data));
    EXPECT_EQ(expected_block_starts,std::get<bed::Col::blockStarts>(bed.data));
    EXPECT_EQ(str, bed.to_string());
}

TEST (BED, BED12_tuple)
{
    using TupleType = std::tuple <std::string, uint32_t, uint32_t, std::string, uint32_t, char, uint32_t, uint32_t,
                                 uint32_t, uint32_t, std::tuple<uint32_t, uint32_t, uint32_t>, std::tuple<uint32_t, uint32_t, uint32_t> >;

    BED<TupleType> bed;
    std::string str = "chr1\t85000835\t85003645\tuc001aaa\t0\t+\t11873\t11873\t0\t3\t354,109,1189,\t0,739,1347,";
    bed.set_bed_data(str);

    auto expected_block_sizes =  std::make_tuple(354, 109, 1189);
    auto expected_block_starts = std::make_tuple(0, 739, 1347);

    
    
    EXPECT_EQ("chr1",               std::get<bed::Col::chrom>(bed.data));
    EXPECT_EQ(85000835,             std::get<bed::Col::chromStart>(bed.data));
    EXPECT_EQ(85003645,             std::get<bed::Col::chromEnd>(bed.data));
    EXPECT_EQ("uc001aaa",           std::get<bed::Col::name>(bed.data));
    EXPECT_EQ(0,                    std::get<bed::Col::score>(bed.data));
    EXPECT_EQ('+',                  std::get<bed::Col::strand>(bed.data));
    EXPECT_EQ(11873,                std::get<bed::Col::thickStart>(bed.data));
    EXPECT_EQ(11873,                std::get<bed::Col::thickEnd>(bed.data));
    EXPECT_EQ(0,                    std::get<bed::Col::itemRgb>(bed.data));
    EXPECT_EQ(3,                    std::get<bed::Col::blockCount>(bed.data));
    EXPECT_EQ(expected_block_sizes, std::get<bed::Col::blockSizes>(bed.data));
    EXPECT_EQ(expected_block_starts,std::get<bed::Col::blockStarts>(bed.data));
    EXPECT_EQ(str, bed.to_string());
}

TEST (BED, set_bed_data)
{   
    
    std::string str("chr1\t88298901\t88300500");
    BED< std::tuple <std::string, uint32_t, uint32_t>> bed;
    bed.set_bed_data(str);

    EXPECT_EQ(str, bed.to_string());
}

TEST (BED, get_obj)
{
    using TupleType = std::tuple <std::string, uint32_t, uint32_t>;
    std::ifstream ifs(file_BED3());
    BED<TupleType > bed;
    BED< TupleType>::get_obj(ifs, bed);

    auto& [ chrom, chrom_start, chrom_end ] = bed.data;
    
    EXPECT_EQ("chr1", chrom);
    EXPECT_EQ(85000835, chrom_start);
    EXPECT_EQ(85003645, chrom_end);
}

TEST (BED, dump)
{
    using TupleType = std::tuple <std::string, uint32_t, uint32_t>;

    std::string ans = "track name=\"ItemRGBDemo\" description=\"Item RGB demonstration\" visibility=2 itemRgb=\"On\"\nchr1\t85000835\t85003645\nchr1\t85100217\t85106585\nchr1\t85153339\t85154239\n";

    std::ifstream ifs(file_BED3_with_header());
    BED< TupleType> bed;
    BEDHeader h;

    std::vector<BED< TupleType>> v_bed;

    ifs>>h;
    // fill BED objects
    while(BED< TupleType>::get_obj(ifs, bed))
    {
        bed.header = h;
        v_bed.emplace_back(std::move(bed));
    }
    

    // dump BED objects to ostream
    std::ostringstream oss;
    BED< TupleType>::dump(oss, v_bed);
 
    EXPECT_EQ( ans, oss.str());
}

TEST (Header_and_BED, iostream)
{
    using TupleType = std::tuple <std::string, uint32_t, uint32_t>;

    std::string ans = "track name=\"ItemRGBDemo\" description=\"Item RGB demonstration\" visibility=2 itemRgb=\"On\"\nchr1\t85000835\t85003645\n";

    std::ifstream ifs(file_BED3_with_header());
    std::ostringstream oss;
    
    BED< TupleType> bed;
    BEDHeader h;

    ifs>>h;
    ifs>>bed;

    oss<<h;
    oss<<bed;

    
    EXPECT_EQ( ans, oss.str());
} 
