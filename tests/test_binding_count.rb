require 'em_test_helper'

class TestBindingCount < Test::Unit::TestCase

  def test_max_bindings
    max = 0
    EM.run {
      max = EM.max_bindings
      EM.stop
    }
    # 384_307_168_202_282_325 on my box, which is a weird number to have as a
    # maximum size...
    assert max > 0
  end

  def test_live_bindings
    live = []
    EM.run {
      EM.heartbeat_interval = 0.01
      EM.start_server("127.0.0.1", 32123)
      live << EM.live_bindings
      20.times do |num|
        c = EM.connect("127.0.0.1", 32123, Module.new)
        live << EM.live_bindings
        instance_variable_set("@c#{num}".to_sym, c)
      end
      EM.stop
    }
    assert_equal (1..21).to_a, live
  end

end
