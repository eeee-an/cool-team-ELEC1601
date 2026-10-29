// =====================================================================
// FORK + SHORT/LONG DEAD END ADDITIONS
// Paste the pieces below into your sketch where indicated.
// =====================================================================


// ---------------------------------------------------------------------
// 1. GLOBALS  (put near your other constants/variables)
//    uTurnSinceLastFork already exists. gotThroughFork / lastForkDecision
//    are no longer needed, you can delete them.
// ---------------------------------------------------------------------

// Dead end length window, in ms of driving since the last turn / fork.
// 1 wall long = short, exactly 2 walls = long, 3+ walls = not long.
// !!! CALIBRATE: see notes at the bottom !!!
const unsigned long LONG_DEAD_END_MIN_MS = 2500;
const unsigned long LONG_DEAD_END_MAX_MS = 4500;

// How far to drive after a side opening is first seen so the robot is
// centred in the opening before turning into it. !!! CALIBRATE !!!
const int FORK_ALIGN_MS = 350;

unsigned long straightStartMs = 0;  // when the current straight run began
int pendingReturnTurn = 0;          // -1 = left, 1 = right (see handleFork / T case)


// ---------------------------------------------------------------------
// 2. NEW HELPER FUNCTIONS  (put with your other functions)
// ---------------------------------------------------------------------

void markStraightStart() {
    straightStartMs = millis();
}

// true if the side is OPEN (no wall)
bool sideClear(bool right) {
    if (right) return irDetect(irLedRight, irSensorRight, 38000) == 1;
    return irDetect(irLedLeft, irSensorLeft, 38000) == 1;
}

// Drive forward until the wall on the open side comes back (we're past the opening)
void passForkForward(bool openOnRight) {
    unsigned long t = millis();
    while (millis() - t < 2000) {
        goForward(100);
        if (!sideClear(openOnRight)) break;                    // wall is back
        if (irDetect(irLedMid, irSensorMid, 38000) == 0) break; // something ahead, stop creeping
    }
}

// Forward-or-side fork. openOnRight = true means: wall on left, opening on right.
//
// Policy (depth first, side branch first):
//   - First time at a fork: take the SIDE branch, remember which way we turned.
//   - If we have just backtracked from a short dead end (uTurnSinceLastFork),
//     the side opening is where we came from, so go FORWARD (unexplored).
void handleFork(bool openOnRight) {
    stop();
    Serial.println(openOnRight ? "FORK: forward or right" : "FORK: forward or left");

    // Confirm it's a real fork and not noise / a corner: nudge forward and re-check
    goForward(100);
    if (!sideClear(openOnRight) || irDetect(irLedMid, irSensorMid, 38000) == 0) {
        return; // false alarm, let loop() re-evaluate what's in front of us
    }

    if (uTurnSinceLastFork) {
        // came back from a dead end -> go straight on
        passForkForward(openOnRight);
        uTurnSinceLastFork = false;
        pendingReturnTurn = 0;
        markStraightStart();
    } else {
        // explore the side branch first
        goForward(FORK_ALIGN_MS);
        if (openOnRight) rightTurn90(); else leftTurn90();
        pendingReturnTurn = openOnRight ? 1 : -1;
        delay(300);
        markStraightStart();          // mark BEFORE driving so the run time counts
        goStraightAfterTurning();
    }
}


// ---------------------------------------------------------------------
// 3. REPLACE your "100: Dead End" block
//    (the   } else if (midDistance < 4)  {   branch inside
//     if (leftBlocked && rightBlocked) { ... }   )
// ---------------------------------------------------------------------

        } else if (midDistance < 4)  {
            // 100: Dead End. Long or short?
            stop();
            unsigned long runMs = millis() - straightStartMs;
            Serial.print("Dead end, run ms = ");
            Serial.println(runMs);

            if (runMs >= LONG_DEAD_END_MIN_MS && runMs <= LONG_DEAD_END_MAX_MS) {
                // LONG dead end = finish. Stay facing forward, do NOT turn or reverse.
                Serial.println("LONG dead end - finished");
                stop();
                showLights(1, 1, 1);
                while (true) { }
            }

            // SHORT dead end: reverse off the wall, spin, and remember we backtracked
            showLights(1, 0, 1);
            delay(500);
            goBackward(300);
            uturn();
            stop();
            uTurnSinceLastFork = true;
            markStraightStart();


// ---------------------------------------------------------------------
// 4. REPLACE your T intersection branch
//    (the   } else if (frontBlocked && !leftBlocked && !rightBlocked) {   branch)
//    Default is a left turn. If we're returning from a side branch we
//    explored first, turn the SAME way we turned into it (that points us
//    down the original forward direction).
// ---------------------------------------------------------------------

    } else if (frontBlocked && !leftBlocked && !rightBlocked) {
        // T intersection
        stop();
        goCloseToWall();
        goBackward(200);

        bool turnRight = false;
        if (uTurnSinceLastFork && pendingReturnTurn != 0) {
            turnRight = (pendingReturnTurn > 0);
            pendingReturnTurn = 0;
            uTurnSinceLastFork = false;
        }
        if (turnRight) rightTurn90(); else leftTurn90();
        delay(300);
        markStraightStart();


// ---------------------------------------------------------------------
// 5. REPLACE the commented-out fork blocks (just before the final else)
// ---------------------------------------------------------------------

    } else if (leftBlocked && !frontBlocked && !rightBlocked) {
        handleFork(true);    // wall on left, opening on right
    } else if (rightBlocked && !frontBlocked && !leftBlocked) {
        handleFork(false);   // wall on right, opening on left


// ---------------------------------------------------------------------
// 6. ADD markStraightStart() AFTER every turn that starts a new straight:
//    - in badStart block, just before goStraightAfterTurning();
//    - in "011: Ideal right turn":  after rightTurn90(); delay(300);
//    - in "010: Ideal left turn":   after leftTurn90();  delay(300);
//    i.e.
//        rightTurn90();
//        delay(300);
//        markStraightStart();          // <-- add
//        goStraightAfterTurning();
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// 7. BUG FIX in goStraightAfterTurning(): these are assignments, not comparisons
//        } else if (rightClear = 1) {   ->   } else if (rightClear == 1) {
//        } else if (leftClear = 1) {    ->   } else if (leftClear == 1) {
// ---------------------------------------------------------------------


// =====================================================================
// CALIBRATION (do this once on the real maze)
// =====================================================================
// 1. Leave the thresholds as-is and run into one SHORT dead end and one
//    LONG dead end. Read "Dead end, run ms = ..." on the serial monitor.
//    Call them S (short) and L (long).
// 2. Set  LONG_DEAD_END_MIN_MS = (S + L) / 2
//         LONG_DEAD_END_MAX_MS = L + (L - S) / 2     // rejects 3-wall dead ends
// 3. FORK_ALIGN_MS: increase until the robot sits centred in the opening
//    before it turns (it should not clip the far wall of the side branch).
// =====================================================================
