#include <iostream>

#include "test_helpers.hxx"

using namespace std;
using namespace pqxx;


// Test program for libpqxx.  Attempt to perform nested queries on various types
// of connections.
namespace
{
void do_test(connection_base &C, const string &desc)
{
  cout << "Testing " << desc << ":" << endl;

  // Trivial test: create subtransactions, and commit/abort
  work T0(C, "T0");
  cout << T0.exec1("SELECT 'T0 starts'")[0].c_str() << endl;
  subtransaction T0a(T0, "T0a");
  T0a.commit();
  subtransaction T0b(T0, "T0b");
  T0b.abort();
  cout << T0.exec1("SELECT 'T0 ends'")[0].c_str() << endl;
  T0.commit();

  // Basic functionality: perform query in subtransaction; abort, continue
  work T1(C, "T1");
  cout << T1.exec1("SELECT 'T1 starts'")[0].c_str() << endl;
  subtransaction T1a(T1, "T1a");
    cout << T1a.exec1("SELECT '  a'")[0].c_str() << endl;
    T1a.commit();
  subtransaction T1b(T1, "T1b");
    cout << T1b.exec1("SELECT '  b'")[0].c_str() << endl;
    T1b.abort();
  subtransaction T1c(T1, "T1c");
    cout << T1c.exec1("SELECT '  c'")[0].c_str() << endl;
    T1c.commit();
  cout << T1.exec1("SELECT 'T1 ends'")[0].c_str() << endl;
  T1.commit();
}


void test_089()
{
  asyncconnection A1;
  do_test(A1, "asyncconnection (virgin)");

  asyncconnection A2;
#include <pqxx/internal/ignore-deprecated-pre.hxx>
  A2.activate();
#include <pqxx/internal/ignore-deprecated-post.hxx>

  do_test(A2, "asyncconnection (initialized)");

  lazyconnection L1;
  do_test(L1, "lazyconnection (virgin)");

  lazyconnection L2;
#include <pqxx/internal/ignore-deprecated-pre.hxx>
  L2.activate();
#include <pqxx/internal/ignore-deprecated-post.hxx>
  do_test(L2, "lazyconnection (initialized)");

  connection C;
#include <pqxx/internal/ignore-deprecated-pre.hxx>
  C.activate();
  C.deactivate();
#include <pqxx/internal/ignore-deprecated-post.hxx>
  do_test(C, "connection (deactivated)");
}
} // namespace


PQXX_REGISTER_TEST(test_089);
