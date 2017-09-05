#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ext");
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", false));
    BOOST_CHECK(GetBoolArg("-ext", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-exto"));
    BOOST_CHECK(!GetBoolArg("-exto", false));
    BOOST_CHECK(GetBoolArg("-exto", true));

    ResetArgs("-ext=0");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", false));
    BOOST_CHECK(!GetBoolArg("-ext", true));

    ResetArgs("-ext=1");
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", false));
    BOOST_CHECK(GetBoolArg("-ext", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noext");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", false));
    BOOST_CHECK(!GetBoolArg("-ext", true));

    ResetArgs("-noext=1");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", false));
    BOOST_CHECK(!GetBoolArg("-ext", true));

    ResetArgs("-ext -noext");  // -ext should win
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", false));
    BOOST_CHECK(GetBoolArg("-ext", true));

    ResetArgs("-ext=1 -noext=1");  // -ext should win
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", false));
    BOOST_CHECK(GetBoolArg("-ext", true));

    ResetArgs("-ext=0 -noext=0");  // -ext should win
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", false));
    BOOST_CHECK(!GetBoolArg("-ext", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ext=1");
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", false));
    BOOST_CHECK(GetBoolArg("-ext", true));

    ResetArgs("--noext=1");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", false));
    BOOST_CHECK(!GetBoolArg("-ext", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ext", "eleven"), "eleven");

    ResetArgs("-ext -bar");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ext", "eleven"), "");

    ResetArgs("-ext=");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ext", "eleven"), "");

    ResetArgs("-ext=11");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ext", "eleven"), "11");

    ResetArgs("-ext=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ext", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ext", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ext", 0), 0);

    ResetArgs("-ext -bar");
    BOOST_CHECK_EQUAL(GetArg("-ext", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ext=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ext", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ext=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ext", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ext");
    BOOST_CHECK_EQUAL(GetBoolArg("-ext"), true);

    ResetArgs("--ext=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ext", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noext");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", true));
    BOOST_CHECK(!GetBoolArg("-ext", false));

    ResetArgs("-noext=1");
    BOOST_CHECK(!GetBoolArg("-ext"));
    BOOST_CHECK(!GetBoolArg("-ext", true));
    BOOST_CHECK(!GetBoolArg("-ext", false));

    ResetArgs("-noext=0");
    BOOST_CHECK(GetBoolArg("-ext"));
    BOOST_CHECK(GetBoolArg("-ext", true));
    BOOST_CHECK(GetBoolArg("-ext", false));

    ResetArgs("-ext --noext");
    BOOST_CHECK(GetBoolArg("-ext"));

    ResetArgs("-noext -ext"); // ext always wins:
    BOOST_CHECK(GetBoolArg("-ext"));
}

BOOST_AUTO_TEST_SUITE_END()
