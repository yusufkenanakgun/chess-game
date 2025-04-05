#include "GameManager.hpp"
#include "PortalSystem.hpp"
#include "unity.h"
#include "unity_fixture.h"

static GameManager* chess;

TEST_GROUP(GameManager);

TEST_SETUP(GameManager)
{
    ConfigReader reader("./data/chess_pieces.json");
    if (!reader.readConfig()) {
        TEST_FAIL_MESSAGE("Failed to read configuration file");
    }

    // Print game settings
    auto settings = reader.getGameSettings();
    chess = new GameManager(settings, reader.getPieceConfigs(), reader.getPortalConfigs());
}

TEST_TEAR_DOWN(GameManager)
{
    delete chess;
}

TEST(GameManager, PlayTurn)
{
    /* Basic turn logic */
    TEST_ASSERT_EQUAL(chess->getCurrentPlayer(), WHITE);
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 0);
    TEST_ASSERT_FALSE(chess->isGameOver());

    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 1), Position(0, 3))); // Move a2 a4
    TEST_ASSERT_NOT_NULL(chess->getBoard().getPieceAtPosition(Position(0, 3)));
    TEST_ASSERT_EQUAL(chess->getCurrentPlayer(), BLACK);
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 1);
    TEST_ASSERT_FALSE(chess->isGameOver());
    
    TEST_ASSERT_FALSE(chess->playTurn(Position(1, 1), Position(1, 3))); // Move b2 b4 (wrong turn)
    TEST_ASSERT_NULL(chess->getBoard().getPieceAtPosition(Position(1, 3)));
    TEST_ASSERT_EQUAL(chess->getCurrentPlayer(), BLACK);
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 1);
    TEST_ASSERT_FALSE(chess->isGameOver());

    TEST_ASSERT_TRUE(chess->playTurn(Position(1, 6), Position(1, 4))); // Move b7 b5
    TEST_ASSERT_NOT_NULL(chess->getBoard().getPieceAtPosition(Position(1, 4)));
    TEST_ASSERT_EQUAL(chess->getCurrentPlayer(), WHITE);
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 2);
    TEST_ASSERT_FALSE(chess->isGameOver());

    /* Piece capturing */
    int pieceCount = chess->getBoard().getPieces().size();
    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 3), Position(1, 4))); // Move a4 b5 (capture black's piece)
    TEST_ASSERT_EQUAL(chess->getCurrentPlayer(), BLACK);
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 3);
    TEST_ASSERT_FALSE(chess->isGameOver());
    TEST_ASSERT_EQUAL(chess->getBoard().getPieces().size(), pieceCount - 1);
}

TEST(GameManager, IsKingUnderCheck)
{
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(WHITE));
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(BLACK));

    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 1), Position(4, 3))); // e2 e4
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 6), Position(5, 4))); // e7 e5
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(WHITE));
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(BLACK));
    
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 0), Position(7, 4)));
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(WHITE));
    TEST_ASSERT_TRUE(chess->isKingUnderCheck(BLACK)); // Black under check
    TEST_ASSERT_FALSE(chess->isGameOver());
}

TEST(GameManager, PlayTurnReverseOnCheck)
{
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 1), Position(4, 3))); // f2 f4
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 6), Position(5, 4))); // f7 f5
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 0), Position(7, 4))); // Queen h5, black under check
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 3);

    TEST_ASSERT_FALSE(chess->playTurn(Position(0, 6), Position(0, 5))); // Black tries a7 a6
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 3);
    TEST_ASSERT_NULL(chess->getBoard().getPieceAtPosition(Position(0, 5))); // Should not have been moved

    TEST_ASSERT_TRUE(chess->playTurn(Position(6, 6), Position(6, 5))); // Black tries g7 g6
    TEST_ASSERT_EQUAL(chess->getMoveCount(), 4);
    TEST_ASSERT_NOT_NULL(chess->getBoard().getPieceAtPosition(Position(6, 5)));
}

TEST(GameManager, FoolsMate)
{
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 1), Position(5, 2)));
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 6), Position(4, 4)));
    TEST_ASSERT_TRUE(chess->playTurn(Position(6, 1), Position(6, 3)));

    TEST_ASSERT_FALSE(chess->isGameOver());
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(WHITE));
    
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 7), Position(7, 3))); // Checkmate by queen
    TEST_ASSERT_TRUE(chess->isGameOver());
    TEST_ASSERT_TRUE(chess->isKingUnderCheck(WHITE));
    TEST_ASSERT_EQUAL(chess->getWinner(), BLACK);
}

TEST(GameManager, ScholarsMate)
{
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 1), Position(4, 3))); // Pawn e2 to e4
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 6), Position(4, 4))); // Pawn e7 to e5
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 0), Position(2, 3))); // Bishop f1 to c4
    TEST_ASSERT_TRUE(chess->playTurn(Position(1, 7), Position(2, 5))); // Knight b8 to c6
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 0), Position(7, 4))); // Queen d1 to h5
    TEST_ASSERT_TRUE(chess->playTurn(Position(6, 7), Position(5, 5))); // Knight h8 to f6

    TEST_ASSERT_FALSE(chess->isGameOver());
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(BLACK));

    TEST_ASSERT_TRUE(chess->playTurn(Position(7, 4), Position(5, 6))); // Queen h5 to f5 (Checkmate)

    TEST_ASSERT_TRUE(chess->isGameOver());
    TEST_ASSERT_TRUE(chess->isKingUnderCheck(BLACK));
    TEST_ASSERT_EQUAL(chess->getWinner(), WHITE);
}

TEST(GameManager, Stalemate)
{
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 1), Position(4, 2))); // Pawn e2 e3
    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 6), Position(0, 4))); // Pawn a7 a5
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 0), Position(7, 4))); // Queen h5
    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 7), Position(0, 5))); // Rook a6
    TEST_ASSERT_TRUE(chess->playTurn(Position(7, 4), Position(0, 4))); // Queen a5
    TEST_ASSERT_TRUE(chess->playTurn(Position(7, 6), Position(7, 4))); // Pawn h7 h5
    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 4), Position(2, 6))); // Queen a5 c7
    TEST_ASSERT_TRUE(chess->playTurn(Position(0, 5), Position(7, 5))); // Rook h6
    TEST_ASSERT_TRUE(chess->playTurn(Position(7, 1), Position(7, 3))); // Pawn h2 h4
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 6), Position(5, 5))); // Pawn f7 f6
    TEST_ASSERT_TRUE(chess->playTurn(Position(2, 6), Position(3, 6))); // Queen d7
    TEST_ASSERT_TRUE(chess->playTurn(Position(4, 7), Position(5, 6))); // King f7
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 6), Position(1, 6))); // Queen b7
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 7), Position(3, 2))); // Queen d3
    TEST_ASSERT_TRUE(chess->playTurn(Position(1, 6), Position(1, 7))); // Queen b8
    TEST_ASSERT_TRUE(chess->playTurn(Position(3, 2), Position(7, 6))); // Queen h7
    TEST_ASSERT_TRUE(chess->playTurn(Position(1, 7), Position(2, 7))); // Queen c8
    TEST_ASSERT_TRUE(chess->playTurn(Position(5, 6), Position(6, 5))); // King g6
    

    TEST_ASSERT_FALSE(chess->isGameOver());
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(BLACK));

    TEST_ASSERT_TRUE(chess->playTurn(Position(2, 7), Position(4, 5))); // Queen e6 (Checkmate)

    TEST_ASSERT_TRUE(chess->isGameOver());
    TEST_ASSERT_FALSE(chess->isKingUnderCheck(BLACK));
    TEST_ASSERT_EQUAL(chess->getWinner(), TIE);
}

TEST_GROUP_RUNNER(GameManager)
{
    RUN_TEST_CASE(GameManager, PlayTurn);
    RUN_TEST_CASE(GameManager, IsKingUnderCheck);
    RUN_TEST_CASE(GameManager, PlayTurnReverseOnCheck);
    RUN_TEST_CASE(GameManager, FoolsMate);
    RUN_TEST_CASE(GameManager, ScholarsMate);
    RUN_TEST_CASE(GameManager, Stalemate);
}