# SPDX-License-Identifier: Apache-2.0

# Delete previous signals on display
gtkwave::/Edit/Highlight_All
gtkwave::/Edit/Delete

# Global signals
set sig_idx 0
set alias_idx 1
set radix_idx 2
set tfile_idx 3
set tfile_path /TranslateFiles/
variable tcl_path [file normalize [info script]]

#### Procedure definitions ####

# Returns path of the tcl script
proc getResourceDirectory {} {
  variable tcl_path
  return [file dirname $tcl_path]
}

# Reloads this Tcl
proc reloadTcl {} {
  global tcl_path
  gtkwave::/File/Read_Tcl_Script_File $tcl_path
}

# Adds a signal list
# Arguments: signal list, color, name for comment trace
proc add_signal_list { sig_list color {topic ""} } {
  global sig_idx
  global alias_idx
  global radix_idx
  global tfile_idx
  global tfile_path
  upvar $sig_list x
  # Add a comment trace if there is one
  if {[string length $topic] != 0} {
    gtkwave::addCommentTracesFromList [list "$topic"]
  }
  foreach signal $x {
    set temp_list [ list ]
    lappend temp_list [lindex $signal $sig_idx]
    gtkwave::addSignalsFromList $temp_list
    gtkwave::highlightSignalsFromList $temp_list
    gtkwave::/Edit/Color_Format/$color
    # Rename the signal if an alias was provided
    if {[string length {lindex $signal $alias_idx}] != 0} {
      gtkwave::/Edit/Alias_Highlighted_Trace [lindex $signal $alias_idx]
      # Needs a re-highlight as the signal name changes
      gtkwave::highlightSignalsFromList [list [lindex $signal $alias_idx]]
    }
    # Set data format
    if {[lindex $signal $radix_idx] ne ""} {
      gtkwave::/Edit/Data_Format/[lindex $signal $radix_idx]
    }
    # Set translation filter file
    if {[lindex $signal $tfile_idx] ne ""} {
      set which_f [ gtkwave::setCurrentTranslateFile [getResourceDirectory]$tfile_path[lindex $signal $tfile_idx]]
      puts $which_f
      gtkwave::installFileFilter $which_f
    }
  }
}

#### Signals ####
# Signal bundles are defined here
# Note that they must still be added to gtkwave with 'add_signal_list' command (see end of file)
#
# On each row:
# First element is name of the signal
# Second element is the alias (how you want it to be displayed in gtkwave)
# Third element is the radix (data format (Binary, Decimal, Hex, ASCII etc.))
# Fourth element is the name of a (possible) Translate File (Stored in ./TranslateFiles/)
#
# After first element, the rest are optional. However, if you want to define fourth element,
# you must also define second and third.
# Note that, when providing a translate file, you should provide a radix that matches with the radix
# used in the translate file

set sys_signals [ list \
  {"ACoreChip.core_clock" clock} \
  {"ACoreChip.core_reset" reset} \
]

set pipeline [ list \
  {"ACoreChip.core.core_state" State Decimal InstructionStage.txt} \
]

set jtag [ list \
  {"ACoreChip.jtag.TAP_controllerInternal.stateMachine.currState" "JTAG State" Hex JTAGFSM.txt} \
  {"ACoreChip.io_jtag_TCK" TCK } \
  {"ACoreChip.io_jtag_TMS" TMS } \
  {"ACoreChip.io_jtag_TDI" TDI } \
  {"ACoreChip.io_jtag_TRSTn" TRSTn } \
  {"ACoreChip.io_jtag_TDO_data" TDO_data } \
  {"ACoreChip.io_jtag_TDO_driven" TDO_driven } \
]

# If you add signals like this, you can comment out unwanted signals
set regfile_int [ list ]
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_0"  x0/zero }
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_1"  x1/ra   } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_2"  x2/sp   } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_3"  x3/gp   } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_4"  x4/tp   } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_5"  x5/t0   } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_6"  x6/t1   } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_7"  x7/t2   } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_8"  x8/s0/fp} 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_9"  x9/s1   } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_10" x10/a0  } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_11" x11/a1  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_12" x12/a2  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_13" x13/a3  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_14" x14/a4  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_15" x15/a5  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_16" x16/a6  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_17" x17/a7  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_18" x18/s2  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_19" x19/s3  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_20" x20/s4  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_21" x21/s5  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_22" x22/s6  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_23" x23/s7  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_24" x24/s8  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_25" x25/s9  } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_26" x26/s10 } 
# lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_27" x27/s11 } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_28" x28/t3  } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_29" x29/t4  } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_30" x30/t5  } 
lappend regfile_int {"ACoreChip.core.regfile_block.regfile.x_31" x31/t6  } 


set regfile_float [ list ]
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_0"  f0/ft0   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_1"  f1/ft1   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_2"  f2/ft2   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_3"  f3/ft3   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_4"  f4/ft4   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_5"  f5/ft5   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_6"  f6/ft6   } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_7"  f7/ft7   } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_8"  f8/fs0   } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_9"  f9/fs1   } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_10" f10/fa0  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_11" f11/fa1  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_12" f12/fa2  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_13" f13/fa3  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_14" f14/fa4  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_15" f15/fa5  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_16" f16/fa6  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_17" f17/fa7  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_18" f18/fs2  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_19" f19/fs3  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_20" f20/fs4  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_21" f21/fs5  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_22" f22/fs6  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_23" f23/fs7  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_24" f24/fs8  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_25" f25/fs9  } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_26" f26/fs10 } 
# lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_27" f27/fs11 } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_28" f28/ft8  } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_29" f29/ft9  } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_30" f30/ft10 } 
lappend regfile_float {"ACoreChip.core.regfile_block.float_regfile.f_31" f31/ft11 } 

set instruction_fetch [ list \
  {"ACoreChip.core.io_ifetch_req" io_ifetch_req } \
  {"ACoreChip.core.io_ifetch_gnt" io_ifetch_gnt } \
  {"ACoreChip.core.io_ifetch_raddr" io_ifetch_raddr } \
  {"ACoreChip.core.io_ifetch_rdata" io_ifetch_rdata } \
  {"ACoreChip.core.io_ifetch_rdata_valid" io_ifetch_rdata_valid } \
]

set decoder [ list \
  {"ACoreChip.core.decoder_block.io_opcode" opcode Binary Opcode.txt} \
]

set lsu [ list ]
lappend lsu {"ACoreChip.core.lsu.io_req"}
lappend lsu {"ACoreChip.core.lsu.io_gnt"}
lappend lsu {"ACoreChip.core.lsu.io_req_info_ren"}
lappend lsu {"ACoreChip.core.lsu.io_req_info_wen"}
lappend lsu {"ACoreChip.core.lsu.io_req_info_op_width" io_info_op_width Decimal MemOpWidth.txt}
lappend lsu {"ACoreChip.core.lsu.io_req_info_addr"}
lappend lsu {"ACoreChip.core.lsu.io_req_info_wdata"}
lappend lsu {"ACoreChip.core.lsu.io_rdata_valid"}
lappend lsu {"ACoreChip.core.lsu.io_rdata_bits"}
lappend lsu {"ACoreChip.core.lsu.io_fault"}

set regfile_write [ list ]
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_data_alu_result"}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_data_imm"}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_data_mem_data"}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_data_rd_waddr"}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_control_wr_en"}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_control_wr_in_select" io_writeIn_control_wr_in_select Decimal RegFileSrc.txt}
lappend regfile_write {"ACoreChip.core.regfile_block.io_writeIn_control_wr_reg_select"}


# You can uncomment any of these if you dont want to see them
add_signal_list sys_signals Green {Core Signals}
add_signal_list jtag Orange {JTAG Signals}
add_signal_list pipeline Orange {Pipeline Signals}
add_signal_list regfile_int Blue {Register File Integer}
add_signal_list regfile_float Yellow {Register File Float}
add_signal_list instruction_fetch Orange {Instruction Fetch}
add_signal_list regfile_write Orange {Register Write}
add_signal_list decoder Violet {Decoder}
add_signal_list lsu Indigo {LSU}


gtkwave::/Edit/UnHighlight_All
#gtkwave::/Time/Zoom/Zoom_Full