project(src)

add_custom_target(ragel_images
    COMMAND ragel -Vp "${CMAKE_CURRENT_SOURCE_DIR}/json.rl" -S number | dot -Tpng > "number.png"
    COMMAND ragel -Vp "${CMAKE_CURRENT_SOURCE_DIR}/json.rl" -S string | dot -Tpng > "string.png"
)

# RAGEL DOCS EXCERPT
#       -T0    (C/D/Java/Ruby/C#) Generate a table driven FSM. This is the default code style.  The table driven FSM represents the state machine  as  static  data.  There  are  tables  of
#              states,  transitions, indicies and actions. The current state is stored in a variable.  The execution is a loop that looks that given the current state and current character
#              to process looks up the transition to take using a binary search, executes any actions and moves to the target state. In general, the table driven  FSM  produces  a  smaller
#              binary and requires a less expensive host language compile but results in slower running code. The table driven FSM is suitable for any FSM.
#
#       -T1    (C/D/Ruby/C#) Generate a faster table driven FSM by expanding action lists in the action execute code.
#
#       -F0    (C/D/Ruby/C#)  Generate  a  flat  table  driven FSM. Transitions are represented as an array indexed by the current alphabet character. This eliminates the need for a binary
#              search to locate transitions and produces faster code, however it is only suitable for small alphabets.
#
#       -F1    (C/D/Ruby/C#) Generate a faster flat table driven FSM by expanding action lists in the action execute code.
#
#       -G0    (C/D/C#) Generate a goto driven FSM. The goto driven FSM represents the state machine as a series of goto statements. While in the machine, the current state  is  stored  by
#              the  processor's instruction pointer. The execution is a flat function where control is passed from state to state using gotos. In general, the goto FSM produces faster code
#              but results in a larger binary and a more expensive host language compile.
#
#       -G1    (C/D/C#) Generate a faster goto driven FSM by expanding action lists in the action execute code.
#
#       -G2    (C/D) Generate a really fast goto driven FSM by embedding action lists in the state machine control code.

#set(FSM_TYPE -T0 -L) # Inhibit writing .rl line numbers for debuggers
set(FSM_TYPE -G2)

add_custom_target(
    ragel_headers
    COMMAND ragel ${FSM_TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/json.rl" -o "${CMAKE_CURRENT_SOURCE_DIR}/parser.hpp"
    COMMAND ragel ${FSM_TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/number_standalone.rl" -o "${CMAKE_CURRENT_SOURCE_DIR}/number.hpp"
    COMMAND ragel ${FSM_TYPE} "${CMAKE_CURRENT_SOURCE_DIR}/string_standalone.rl" -o "${CMAKE_CURRENT_SOURCE_DIR}/string.hpp"
    DEPENDS json.rl
)
