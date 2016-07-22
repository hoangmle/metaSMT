#define BOOST_TEST_MODULE direct_CVC4
#include <metaSMT/DirectSolver_Context.hpp>
#include <metaSMT/backend/Yices2.hpp>

using namespace metaSMT;
struct Solver_Fixture {
  typedef DirectSolver_Context<metaSMT::solver::Yices2> ContextType;
  ContextType ctx;
};

#include "test_solver.cpp"
#include "test_QF_BV.cpp"
#include "test_cardinality.cpp"
#include "test_Array.cpp"
