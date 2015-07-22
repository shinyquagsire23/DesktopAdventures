### IACT and Scripts

Scripts in the game are formatted in a cause-effect format. The triggers section is the section which determines the prerequisites for a script to be fired, with the script section being fired if the prerequisites are fulfilled. As of now, about 90% of the script commands have been documented through straight disassembly of the trigger/script routines at 0x407E90 and 0x4087F0 respectively. Based on a small initial analysis, it seems the Yoda Stories scripting commands are in line with the ones in Indianna Jones, however this may or may not be the case when it comes down to arguments.

The actual formatting of the scripts in each IZON is as follows:

- u16 number of scripts
  - u32 'IACT'
    - u32 unknown/unused?
    - u16 trigger count
      - u16 trigger command
      - u16 arg1
      - u16 arg2
      - u16 arg3
      - u16 arg4
      - u16 arg5
      - u16 arg6
      - u16 second trigger command
      - ...
    - u16 script command count
      - u16 script command
      - u16 arg1
      - u16 arg2
      - u16 arg3
      - u16 arg4
      - u16 arg5
      - u16 strlen
      - char* string (based on strlen, but only actually used/skipped over in commands which use it)
      - u16 second script command
      - ...
  - u32 'IACT'
    - ...
    
## Triggers and Commands
Documentation on each trigger and command can be found in scrdoc.txt.
