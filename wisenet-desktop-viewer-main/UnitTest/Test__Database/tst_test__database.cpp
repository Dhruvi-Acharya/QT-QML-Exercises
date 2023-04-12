#include "tst_test__database.h"
#include <boost/core/ignore_unused.hpp>


Test__Database::Test__Database()
    :m_database(std::make_shared<Wisenet::Core::DatabaseManager>())
{

}

Test__Database::~Test__Database()
{

}

void Test__Database::initTestCase()
{
    Wisenet::Common::InitializeLogSettings("logs");
    QVERIFY(m_database->Delete());

    QVERIFY(m_database->Open());
}

void Test__Database::cleanupTestCase()
{
    m_database->Close();
}

// debugging with standalone mode
QTEST_MAIN(Test__Database)

// run unit test
//QTEST_APPLESS_MAIN(Test__Database)

//#include "tst_test__database.moc"
