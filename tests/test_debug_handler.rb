require 'em_test_helper'

class TestDebugHandler < Test::Unit::TestCase
  def setup
    @values = []

    EM.debug_handler lambda {|signature, connection, event_type,
                              last_activity_time, loop_start_time,
                              event_duration, data|
      now = Time.now
      value = {
        :signature => signature,
        :connection => connection,
        :event_type => event_type,
        :last_activity_time => last_activity_time,
        :loop_start_time => loop_start_time,
        :current_loop_duration => (now - loop_start_time) * 1000,
        :event_duration => event_duration
      }
      if last_activity_time
        value[:connection_idle_time] = (now - last_activity_time) * 1000
      end
      value[:data] = data

      @values << value
    }
  end

  def test_debug_handler
    EM.run {
      EM.add_timer(0) {
        EM.debug_handler(nil)
        EM.stop
      }
    }

    event = @values.first

    assert_equal 0, event[:signature]
    assert_equal :EM_TIMER_FIRED, event[:event_type]
    assert_kind_of Time, event[:loop_start_time]
  end

  def test_debug_handler_duration
    EM.run {
      sleep 0.2
      EM.add_timer(0) {
        EM.debug_handler(nil)
        EM.stop
      }
    }

    event = @values.first
    assert event[:event_duration] >= 200
  end

end
