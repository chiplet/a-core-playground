package acoreplayground

import chisel3._
import chisel3.util._
import chisel3.stage.{ChiselStage, ChiselGeneratorAnnotation}

class Blinky extends Module {

  val io = IO(new Bundle {
    val led = Output(Bool())
  })

  val reg = RegInit(0.U(8.W))
  reg := reg + 1.U

  io.led := reg(7)

}

object Blinky extends App {
  val annos = Seq(ChiselGeneratorAnnotation(() => new Blinky()))
  (new ChiselStage).execute(args, annos)
}
