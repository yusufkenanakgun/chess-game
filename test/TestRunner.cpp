#include "unity_fixture.h"

static void RunAllTests(void)
{
  RUN_TEST_GROUP(ChessBoard);
  RUN_TEST_GROUP(MoveValidator);
  RUN_TEST_GROUP(PortalSystem);
  RUN_TEST_GROUP(GameManager);
}

int main(int argc, const char * argv[])
{
  return UnityMain(argc, argv, RunAllTests);
}