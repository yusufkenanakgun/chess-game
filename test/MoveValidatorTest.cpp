#include "ChessBoard.hpp"
#include "MoveValidator.hpp"
#include "unity.h"
#include "unity_fixture.h"

static ChessBoard* board;
static MoveValidator* validator;

TEST_GROUP(MoveValidator);

TEST_SETUP(MoveValidator)
{
    ConfigReader reader("./data/chess_nopawn.json"); // No Pawns!!
    if (!reader.readConfig()) {
        TEST_FAIL_MESSAGE("Failed to read configuration file");
    }

    // Print game settings
    auto settings = reader.getGameSettings();
    board = new ChessBoard(settings, reader.getPieceConfigs());
    validator = new MoveValidator(*board, reader.getPieceConfigs());
}

TEST_TEAR_DOWN(MoveValidator)
{
    delete board;
    delete validator;
}

TEST(MoveValidator, PossibleMoves)
{
    for (const ChessPiece& piece : board->getPieces()) {
        // Get all possible moves according to validator
        std::set<Position> moves = validator->getPossibleMoves(piece);

        // Check all board locations, ensure valid ones are
        // already in moves returned by validator
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Position pos(i, j);
                if (validator->validateMove(piece, pos)) {
                    TEST_ASSERT_EQUAL(moves.count(pos), 1);
                }
            }
        }
    }
}

#define p(x, y) Position((x),(y))
TEST(MoveValidator, ValidateMove)
{
    std::map<ChessPiece*, std::set<Position>> pieceMoves;

    /* Rooks */
    // White Left
    pieceMoves[board->getPieceAtPosition(Position(0, 0))]
        = std::set { p(0,1), p(0,2), p(0,3), p(0,4), p(0,5), p(0,6), p(0,7) };
    // Black Left
    pieceMoves[board->getPieceAtPosition(Position(0, 7))]
        = std::set { p(0,0), p(0,1), p(0,2), p(0,3), p(0,4), p(0,5), p(0,6) };
    // White Right
    pieceMoves[board->getPieceAtPosition(Position(7, 0))]
        = std::set { p(7,1), p(7,2), p(7,3), p(7,4), p(7,5), p(7,6), p(7,7) };
    // Black Right
    pieceMoves[board->getPieceAtPosition(Position(7, 7))]
        = std::set { p(7,0), p(7,1), p(7,2), p(7,3), p(7,4), p(7,5), p(7,6) };

    /* Bishops */
    // White Left
    pieceMoves[board->getPieceAtPosition(Position(2, 0))]
        = std::set { p(1,1), p(0,2), p(3,1), p(4,2), p(5,3), p(6,4), p(7,5) };
    // Black Left
    pieceMoves[board->getPieceAtPosition(Position(2, 7))]
        = std::set { p(1,6), p(0,5), p(3,6), p(4,5), p(5,4), p(6,3), p(7,2) };
    // White Right
    pieceMoves[board->getPieceAtPosition(Position(5, 0))]
        = std::set { p(6,1), p(7,2), p(4,1), p(3,2), p(2,3), p(1,4), p(0,5) };
    // Black Right
    pieceMoves[board->getPieceAtPosition(Position(5, 7))]
        = std::set { p(6,6), p(7,5), p(4,6), p(3,5), p(2,4), p(1,3), p(0,2) };

    /* Knights */
    // White Left
    pieceMoves[board->getPieceAtPosition(Position(1, 0))]
        = std::set { p(0,2), p(2,2), p(3,1) };
    // Black Left
    pieceMoves[board->getPieceAtPosition(Position(1, 7))]
        = std::set { p(0,5), p(2,5), p(3,6) };
    // White Right
    pieceMoves[board->getPieceAtPosition(Position(6, 0))]
        = std::set { p(7,2), p(5,2), p(4,1) };
    // Black Right
    pieceMoves[board->getPieceAtPosition(Position(6, 7))]
        = std::set { p(7,5), p(5,5), p(4,6) };

    /* Queens */
    // White
    pieceMoves[board->getPieceAtPosition(Position(3, 0))]
        = std::set { p(3,1), p(3,2), p(3,3), p(3,4), p(3,5), p(3,6), p(3,7),
                    p(2,1), p(1,2), p(0,3), p(4,1), p(5,2), p(6,3), p(7,4) };
    // Black
    pieceMoves[board->getPieceAtPosition(Position(3, 7))]
        = std::set { p(3,6), p(3,5), p(3,4), p(3,3), p(3,2), p(3,1), p(3,0),
                    p(2,6), p(1,5), p(0,4), p(4,6), p(5,5), p(6,4), p(7,3) };

    /* Kings */
    // White
    pieceMoves[board->getPieceAtPosition(Position(4, 0))]
        = std::set { p(3,1), p(4,1), p(5,1) };
    // Black
    pieceMoves[board->getPieceAtPosition(Position(4, 7))]
        = std::set { p(3,6), p(4,6), p(5,6) };


    for (const auto& movesPair : pieceMoves) {
        TEST_ASSERT_NOT_NULL(movesPair.first);
        const ChessPiece& piece = *movesPair.first;
        const std::set<Position>& moves = movesPair.second;

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Position pos(i, j);

                if (validator->validateMove(piece, pos)) {
                    // We found valid move, ensure it is in moves
                    std::string failMessage = piece.type + ": did not expect valid move at (" + 
                        std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";

                    TEST_ASSERT_EQUAL_MESSAGE(moves.count(pos), 1, failMessage.c_str());
                } else {
                    // We found invalid move, ensure it is _not_ in moves
                    std::string failMessage = piece.type + ": did not expect invalid move at (" + 
                        std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")";

                    TEST_ASSERT_EQUAL_MESSAGE(moves.count(pos), 0, failMessage.c_str());
                }
            }
        }
    }
}
#undef p

TEST(MoveValidator, ValidatePortalUse)
{
    Portal portal("P", Position(3, 3), Position(3, 4), false, true, false, 3);
    
    board->addPortal(portal);
    TEST_ASSERT_TRUE(validator->validatePortalUse(*board->getPieceAtPosition(Position(0,0)), portal)); // White can use
    TEST_ASSERT_FALSE(validator->validatePortalUse(*board->getPieceAtPosition(Position(0,7)), portal)); // Black can't
    portal.current_cooldown = portal.cooldown;
    TEST_ASSERT_FALSE(validator->validatePortalUse(*board->getPieceAtPosition(Position(0,0)), portal));
}

TEST_GROUP_RUNNER(MoveValidator)
{
    RUN_TEST_CASE(MoveValidator, PossibleMoves);
    RUN_TEST_CASE(MoveValidator, ValidateMove);
    RUN_TEST_CASE(MoveValidator, ValidatePortalUse);
}