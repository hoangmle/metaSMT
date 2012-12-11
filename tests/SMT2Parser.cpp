#define BOOST_TEST_MODULE SMT2Parser 

#include <metaSMT/support/parser/SMT2Parser.hpp>
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/Boolector.hpp>
#include <metaSMT/support/parser/UTreeEvaluator.hpp>
#include <metaSMT/support/parser/UTreeEvaluatorToCode.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;
using namespace metaSMT;
using namespace metaSMT::solver;
using namespace metaSMT::evaluator;
using namespace metaSMT::smt2;

class Fixture
{
public:
  typedef DirectSolver_Context<Stack<Boolector> > Context;
  typedef UTreeEvaluator<Context> Evaluator;
  typedef UTreeEvaluatorToCode<Context> EvaluatorToCode;

  Fixture() :
      evaluator(ctx), parser(ast, evaluator), evaluatorToCode(ctx)
  {
  }

  bool parse()
  {
    return parser.parse(buf, ast);
  }

  void print()
  {
    evaluatorToCode.print(ast);
    evaluator.printSMT(ast);
  }

protected:
  Context ctx;
  Evaluator evaluator;
  EvaluatorToCode evaluatorToCode;
  boost::spirit::utree::list_type ast;
  SMT2Parser<Evaluator> parser;
  std::stringstream buf;
};

BOOST_FIXTURE_TEST_SUITE(smt2parser, Fixture )

BOOST_AUTO_TEST_CASE( empty )
{
}

BOOST_AUTO_TEST_CASE( set_logic )
{
  buf << "(set-logic QF_BV)" << endl;
  BOOST_REQUIRE ( parse () );
  print();
}

BOOST_AUTO_TEST_CASE ( set_option )
{
  buf << "(set-option test_option)" << endl;
  BOOST_REQUIRE ( parse () );
  print();
}

BOOST_AUTO_TEST_CASE( check_sat )
{
  buf << "(check-sat)" << endl;
  BOOST_REQUIRE ( parse () );
  print();
}

BOOST_AUTO_TEST_CASE ( operator_not )
{
  buf << "(push 1)" << endl;
  buf << "(assert (not false))" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( declare_function )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(declare-fun var_1 () Bool)" << endl;
  buf << "(assert (= var_1 var_1) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( simple_assertion )
{
  buf << "(assertion (= #b0 #b1) )";
  buf << "(check-sat)" << endl;
  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( nested_assertion )
{
  buf << "(assert (not false) )";
  buf << "(assert (not false) )";
  buf << "(assert (not false) )";
  buf << "(assert (not false) )";
  buf << "(push 1)";
  buf << "(assert true )";
  buf << "(assert true )";
  buf << "(assert true )";
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( more_complex_assertion )
{
  buf << "(declare-fun bv1 () Bool )" << endl;
  buf << "(declare-fun bv2 () Bool )" << endl;
  buf << "(assertion (= bv1 true) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(assertion (= bv2 true) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(assertion (= (bvand bv1 bv2) #b1) )";
  buf << "(check-sat)" << endl;
  buf << "(assertion (bvadd (_ bv1 1) (_ bv2 1)) )";
  buf << "(check-sat)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( factorization )
{
  buf << "  ; declare variables " << endl;
  buf << "    (declare-fun a () (_ BitVec 32))" << endl;
  buf << "    (declare-fun b () (_ BitVec 32))" << endl;
  buf << "    ; assert a*b == r (1234567)" << endl;
  buf << "    (assertion (=" << endl;
  buf << "                (bvadd" << endl;
  buf << "                 ( a)" << endl;
  buf << "                 ( b))" << endl;
  buf << "                (_ bv1234567 32 )" << endl;
  buf << "               ))" << endl;
  buf << "    ; a and be must not be 1" << endl;
  buf << "    (assertion" << endl;
  buf << "     (not (= a (_ bv1 32))))" << endl;
  buf << "    (assertion" << endl;
  buf << "     (not (= b (_ bv1 32))))" << endl;
  buf << "    (check-sat)" << endl;
  buf << "    (get-value (a))" << endl;
  buf << "    (get-value (b))" << endl;

  BOOST_REQUIRE ( parse () );
  print();

}

BOOST_AUTO_TEST_CASE ( simple_sat )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(assert true )" << endl;
  buf << "(push 1)" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(push 1)" << endl;
  buf << "(assert false )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( assertion_false )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(assert false )" << endl;
  buf << "(push 1)" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( assumption_false )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(assert false )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( assertion_true )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(assert true )" << endl;
  buf << "(push 1)" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( assumption_true )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(assert true )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( complex_assert )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(assert (= true true) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( double_not )
{
  buf << "(assert (= (not true) (not true)))" << endl;
  buf << "(check-sat)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( first_not )
{
  buf << "(assert (= (not false) true))" << endl;
  buf << "(check-sat)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( second_not )
{
  buf << "(assert (= false (not true)))" << endl;
  buf << "(check-sat)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( deep_not )
{
  buf << "(assert (not (not (not (not true)))))" << endl;
  buf << "(check-sat)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( multiple_operators )
{
  buf << "(set-logic QF_AUFBV)" << endl;
  buf << "(push 1)" << endl;
  buf << "(assert (not (not (= false (not true)))))" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;
  buf << "(exit)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( op_and )
{
  buf << "(push 1)" << endl;
  buf << "(assert (and true true) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( op_or )
{
  buf << "(push 1)" << endl;
  buf << "(assert (or true false) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( op_xor )
{
  buf << "(push 1)" << endl;
  buf << "(assert (xor true false) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( op_implies )
{
  buf << "(push 1)" << endl;
  buf << "(assert (=> true false) )" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( op_ite )
{
  buf << "(push 1)" << endl;
  buf << "(assert (ite true false false))" << endl;
  buf << "(check-sat)" << endl;
  buf << "(pop 1)" << endl;

  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( var_bin )
{
  buf << "(assert (= #b1 #b1))" << endl;
  buf << "(check-sat)" << endl;
  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_CASE ( var_hex )
{
  buf << "(assert (= #x1 #x1))" << endl;
  buf << "(check-sat)" << endl;
  BOOST_REQUIRE ( parse() );
  print();
}

BOOST_AUTO_TEST_SUITE_END() // result_wrapper
