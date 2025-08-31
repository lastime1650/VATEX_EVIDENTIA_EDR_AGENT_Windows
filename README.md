# VATEX_EVIDENTIA_EDR_AGENT_Windows

## Compile Error?

### 1. Linker Problem

Kernel Project -> Linker -> Input -> Additional Dependencies

and... Copy it
`$(DDK_LIB_PATH)netio.lib
$(DDK_LIB_PATH)ndis.lib
$(DDK_LIB_PATH)wdm.lib
$(DDK_LIB_PATH)fwpkclnt.lib
$(DDK_LIB_PATH)wdmsec.lib
$(DDK_LIB_PATH)ksecdd.lib
$(DDK_LIB_PATH)cng.lib
$(DDK_LIB_PATH)fltMgr.lib`
