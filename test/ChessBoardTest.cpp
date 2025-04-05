#include "ChessBoard.hpp"
#include "unity.h"
#include "unity_fixture.h"

static ChessBoard* board;

TEST_GROUP(ChessBoard);

TEST_SETUP(ChessBoard)
{
    ConfigReader reader("./data/chess_pieces.json");
    if (!reader.readConfig()) {
        TEST_FAIL_MESSAGE("Failed to read configuration file");
    }

    // Print game settings
    auto settings = reader.getGameSettings();
    board = new ChessBoard(settings, reader.getPieceConfigs());
}

TEST_TEAR_DOWN(ChessBoard)
{
    delete board;
}

TEST(ChessBoard, BoardInitialization)
{
    // Test board size
    TEST_ASSERT_EQUAL(board->getSize(), 8);

    // Test piece count
    TEST_ASSERT_EQUAL(board->getPieces().size(), 32);

    // Test portals, must be 0 (non-fantasy)
    TEST_ASSERT_EQUAL(board->getPortals().size(), 0);

    // Test kings
    TEST_ASSERT_NOT_NULL(board->getKingOfTeam(WHITE));
    TEST_ASSERT_NOT_NULL(board->getKingOfTeam(BLACK));
    TEST_ASSERT_EQUAL(board->getKingOfTeam(WHITE)->team, WHITE);
    TEST_ASSERT_EQUAL(board->getKingOfTeam(BLACK)->team, BLACK);

    // Test pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* nPawn = board->getPieceAtPosition(Position(i, 6));
        ChessPiece* sPawn = board->getPieceAtPosition(Position(i, 1));
        TEST_ASSERT_NOT_NULL(nPawn);
        TEST_ASSERT_EQUAL(nPawn->team, BLACK); // North is black team
        TEST_ASSERT_NOT_NULL(sPawn);
        TEST_ASSERT_EQUAL_STRING(nPawn->type.c_str(), "pawn");
        TEST_ASSERT_EQUAL(sPawn->team, WHITE); // South is white team
        TEST_ASSERT_EQUAL_STRING(sPawn->type.c_str(), "pawn");
    }
}

TEST(ChessBoard, MovePiece)
{
    // Move pawns
    for (int i = 0; i < 8; i++) {
        ChessPiece* nPawn = board->getPieceAtPosition(Position(i, 6));
        ChessPiece* sPawn = board->getPieceAtPosition(Position(i, 1));

        board->movePiece(*nPawn, Position(i, 4));
        board->movePiece(*sPawn, Position(i, 3));

        Position nPos(i, 4);
        Position sPos(i, 3);

        // Struct equality
        TEST_ASSERT_EQUAL_MEMORY(&nPawn->position, &nPos, sizeof(Position));
        TEST_ASSERT_EQUAL_MEMORY(&sPawn->position, &sPos, sizeof(Position));

        // Pawns should have disappeared from their initial place
        TEST_ASSERT_NULL(board->getPieceAtPosition(Position(i, 6)));
        TEST_ASSERT_NULL(board->getPieceAtPosition(Position(i, 1)));
    }

    try {
        // Pieces can not be moved to existing piece locations
        ChessPiece* rook = board->getPieceAtPosition(Position(0, 0));
        board->movePiece(*rook, Position(0, 7));
        TEST_FAIL_MESSAGE("Board should have thrown an error");
    } catch (const std::runtime_error& err) { }
}

TEST(ChessBoard, RemovePiece)
{
    ChessPiece* piece = board->getPieceAtPosition(Position(0, 1));
    TEST_ASSERT_NOT_NULL(piece);

    board->removePiece(piece);

    TEST_ASSERT_NULL(board->getPieceAtPosition(Position(0, 1)));
    std::list<ChessPiece*> teamPieces = board->getPiecesOfTeam(piece->team);
    for (const ChessPiece* teamPiece : teamPieces) {
        TEST_ASSERT_NOT_EQUAL(teamPiece, piece);
    }
}

TEST(ChessBoard, AddPiece)
{
    ChessPiece newPiece("queen", false, Position(4, 4), BLACK);
    board->addPiece(newPiece);

    ChessPiece* piece = board->getPieceAtPosition(Position(4, 4));
    TEST_ASSERT_NOT_NULL(piece);
    TEST_ASSERT_EQUAL_STRING(piece->type.c_str(), "queen");
    TEST_ASSERT_EQUAL(piece->team, BLACK);
}

TEST(ChessBoard, AddPortal)
{
    Portal newPortal("X", Position(4, 4), Position(7, 7), false, true, true, 0);
    board->addPortal(newPortal);

    Portal* portal = board->getPortalAtPosition(Position(4, 4));
    TEST_ASSERT_NOT_NULL(portal);
    Position ePos = Position(7, 7);
    TEST_ASSERT_EQUAL_MEMORY(&portal->exit, &ePos, sizeof(Position));
}

TEST(ChessBoard, GetPieceAtPosition)
{
    ChessPiece* piece = board->getPieceAtPosition(Position(0, 0));
    TEST_ASSERT_NOT_NULL(piece);
    TEST_ASSERT_EQUAL_STRING(piece->type.c_str(), "rook");

    ChessPiece* emptyPiece = board->getPieceAtPosition(Position(5, 5));
    TEST_ASSERT_NULL(emptyPiece);
}

TEST(ChessBoard, GetKingOfTeam)
{
    ChessPiece* whiteKing = board->getKingOfTeam(WHITE);
    TEST_ASSERT_NOT_NULL(whiteKing);
    TEST_ASSERT_EQUAL_STRING(whiteKing->type.c_str(), "King");
    TEST_ASSERT_TRUE(whiteKing->king_type);
    TEST_ASSERT_EQUAL(whiteKing->team, WHITE);

    ChessPiece* blackKing = board->getKingOfTeam(BLACK);
    TEST_ASSERT_NOT_NULL(blackKing);
    TEST_ASSERT_EQUAL_STRING(blackKing->type.c_str(), "King");
    TEST_ASSERT_TRUE(whiteKing->king_type);
    TEST_ASSERT_EQUAL(blackKing->team, BLACK);
}

TEST(ChessBoard, ExchangePiecePositions)
{
    ChessPiece* piece = board->getPieceAtPosition(Position(0, 0));
    TEST_ASSERT_NOT_NULL(piece);
    ChessPiece* other = board->getPieceAtPosition(Position(0, 7));
    TEST_ASSERT_NOT_NULL(other);

    board->exchangePiecePositions(*piece, *other);

    Position nPos = Position(0, 7);
    Position sPos = Position(0, 0);
    TEST_ASSERT_EQUAL_MEMORY(&piece->position, &nPos, sizeof(Position));
    TEST_ASSERT_EQUAL_MEMORY(&other->position, &sPos, sizeof(Position));
}

TEST_GROUP_RUNNER(ChessBoard)
{
  RUN_TEST_CASE(ChessBoard, BoardInitialization);
  RUN_TEST_CASE(ChessBoard, MovePiece);
  RUN_TEST_CASE(ChessBoard, RemovePiece);
  RUN_TEST_CASE(ChessBoard, AddPiece);
  RUN_TEST_CASE(ChessBoard, AddPortal);
  RUN_TEST_CASE(ChessBoard, GetPieceAtPosition);
  RUN_TEST_CASE(ChessBoard, GetKingOfTeam);
  RUN_TEST_CASE(ChessBoard, ExchangePiecePositions);
}