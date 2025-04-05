#include "ChessBoard.hpp"
#include "PortalSystem.hpp"
#include "unity.h"
#include "unity_fixture.h"

static ChessBoard* board;
static PortalSystem* portal_system;

TEST_GROUP(PortalSystem);

TEST_SETUP(PortalSystem)
{
    ConfigReader reader("./data/fantasy_chess.json"); // Fantasy world with portals
    if (!reader.readConfig()) {
        TEST_FAIL_MESSAGE("Failed to read configuration file");
    }

    // Print game settings
    auto settings = reader.getGameSettings();
    board = new ChessBoard(settings, reader.getPieceConfigs());
    portal_system = new PortalSystem(*board, reader.getPortalConfigs());
}

TEST_TEAR_DOWN(PortalSystem)
{
    delete board;
    delete portal_system;
}

TEST(PortalSystem, InitializePortals)
{
    TEST_ASSERT_EQUAL(board->getPortals().size(), 3);

    Portal* portalX = board->getPortalAtPosition(Position(0, 5));
    TEST_ASSERT_NOT_NULL(portalX);
    TEST_ASSERT_EQUAL_STRING(portalX->id.c_str(), "X");
    TEST_ASSERT_EQUAL(portalX->cooldown, 5);
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 0);
    
    Portal* portalY = board->getPortalAtPosition(Position(7, 3));
    TEST_ASSERT_NOT_NULL(portalY);
    TEST_ASSERT_EQUAL_STRING(portalY->id.c_str(), "Y");
    TEST_ASSERT_EQUAL(portalY->cooldown, 1);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    
    Portal* portalZ = board->getPortalAtPosition(Position(3, 3));
    TEST_ASSERT_NOT_NULL(portalZ);
    TEST_ASSERT_EQUAL_STRING(portalZ->id.c_str(), "Z");
    TEST_ASSERT_EQUAL(portalZ->cooldown, 3);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 0);
}

TEST(PortalSystem, StartCooldown)
{
    Portal* portalX = board->getPortalAtPosition(Position(0, 5));
    TEST_ASSERT_NOT_NULL(portalX);
    portal_system->startCooldown(portalX->entry);
    TEST_ASSERT_EQUAL(portalX->current_cooldown, portalX->cooldown);

    Portal* portalY = board->getPortalAtPosition(Position(7, 3));
    TEST_ASSERT_NOT_NULL(portalY);
    portal_system->startCooldown(portalY->entry);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, portalY->cooldown);
    
    Portal* portalZ = board->getPortalAtPosition(Position(3, 3));
    TEST_ASSERT_NOT_NULL(portalZ);
    portal_system->startCooldown(portalZ->entry);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, portalZ->cooldown);
}

TEST(PortalSystem, DecreaseCooldowns)
{

    Portal* portalX = board->getPortalAtPosition(Position(0, 5));
    TEST_ASSERT_NOT_NULL(portalX);
    portal_system->startCooldown(portalX->entry);

    Portal* portalY = board->getPortalAtPosition(Position(7, 3));
    TEST_ASSERT_NOT_NULL(portalY);
    portal_system->startCooldown(portalY->entry);
    
    Portal* portalZ = board->getPortalAtPosition(Position(3, 3));
    TEST_ASSERT_NOT_NULL(portalZ);
    portal_system->startCooldown(portalZ->entry);

    portal_system->decreaseCooldowns();
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 4);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 2);

    portal_system->decreaseCooldowns();
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 3);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 1);

    portal_system->decreaseCooldowns();
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 2);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 0);

    portal_system->decreaseCooldowns();
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 1);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 0);

    portal_system->decreaseCooldowns();
    TEST_ASSERT_EQUAL(portalX->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalY->current_cooldown, 0);
    TEST_ASSERT_EQUAL(portalZ->current_cooldown, 0);
}

TEST_GROUP_RUNNER(PortalSystem)
{
    RUN_TEST_CASE(PortalSystem, InitializePortals);
    RUN_TEST_CASE(PortalSystem, StartCooldown);
    RUN_TEST_CASE(PortalSystem, DecreaseCooldowns);
}