/// @file header.hpp
/// @brief Header of BED object 

#pragma once
#include <sstream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/tokenizer.hpp>
#include <map>
#include <vector>
#include <cassert>
#include <string>

namespace biovoltron::format::bed {
    class Header
    {
      public:

        Header():item_RGB(false), 
                 use_score(false),
                 color_by_strand(false)
                 {}

        


        void set(std::string& str)
        {
            using boost::tokenizer;
            using boost::escaped_list_separator;
            using so_tokenizer = tokenizer<escaped_list_separator<char> >;
            
            track_line = str;

            so_tokenizer tok(str, escaped_list_separator<char>("\\", " \t=", "\""));
            
            assert( *tok.begin() == "track" );
            

            for( auto it  = tok.begin();
                      it != tok.end(); 
                      ++it)
            {
                if(*it == "colorByStrand")
                {
                    color_by_strand = true;
                    ++it;
                    //boost::spirit::qi::phrase_parse(it->begin(), it->end(), boost::spirit::qi::int_ % ',', boost::spirit::ascii::space, 
                                                    //forward_strand_color);
                    std::vector<std::string> split_string;

                    boost::split( split_string, *it, boost::is_any_of( " \t," ), boost::token_compress_on );

                    for(int i = 0; i < 3; i++)
                        forward_strand_color.emplace_back(stoi(split_string[i]));
                    for(int i = 3; i < 6; i++)
                        reverse_strand_color.emplace_back(stoi(split_string[i]));
                    
                }
                else if(*it == "useScore")
                {
                    ++it;
                    use_score = ( *it == "0" ) ? false : true;
                }
                else if(*it == "itemRgb")
                {
                    ++it;
                    if(*it == "On")
                        item_RGB = true;
                    else
                        item_RGB = false;
                }
                else if(*it == "name")
                {
                    name = *(++it);
                }
                else if(*it == "description")
                {
                    description = *(++it);
                }
                else if(*it == "visibility")
                {
                    ++it;
                    visibility = stoi(*it);
                    //boost::spirit::qi::phrase_parse(it->begin(), it->end(), boost::spirit::qi::int_, boost::spirit::ascii::space, 
                                                    //visibility);
                }
            }
        }

        friend std::istream& operator>>(std::istream& is, Header& header)
        {
            std::string str;
            std::getline(is, str);
            header.set(str);
            return is;
        }

        friend std::ostream& operator<<(std::ostream& os, const Header& header)
        {
            os << header.to_string()<<"\n";
            return os;
        }

        std::string to_string() const
        {
            return track_line;
        }

        

        
        std::string  track_line, name, description;
        uint32_t visibility;
        bool item_RGB, use_score, color_by_strand;
        std::vector<uint8_t> forward_strand_color, reverse_strand_color;
    };

    enum Col
    {
        chrom, chromStart, chromEnd, name, score, strand, thickStart, thickEnd, itemRgb, blockCount, blockSizes, blockStarts
    };

}
