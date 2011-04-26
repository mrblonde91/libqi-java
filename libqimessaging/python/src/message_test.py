#!/usr/bin/env python
##
## Author(s):
##  - Cedric GESTES <gestes@aldebaran-robotics.com>
##
## Copyright (C) 2011 Aldebaran Robotics
##

import qi.message

def test_message():
    msg = qi.message.Message()

    # msg.write_bool(1)
    # msg.write_char("c")
    # msg.write_int(42)
    msg.write_float(42.0)
    msg.write_double(42.0)
    # msg.write_string("str")

    ret = qi.message.message_to_python("fd", msg)
    #ret = qi.message.message_to_python("bcifds", msg)
    print "bim"
    print "ret:", ret
    assert(ret == ( 42.0, 42.0 ))

    msg2 = qi.Message()
    #qi.message.python_to_message("fd", msg2, 42.0, 42.0)
    qi.message.python_to_message("fd", msg2, *ret)
    print "msg2"
    ret2 = qi.message.message_to_python("fd", msg2)
    print "ret2", ret2
    assert(ret2 == ( 42.0, 42.0 ))
    print "ret2:", ret


def test_message2():
    msg = qi.message.Message()

    msg.write_bool(1)
    msg.write_char("c")
    msg.write_int(42)
    msg.write_float(42.0)
    msg.write_double(42.0)
    msg.write_string("str")

    #array [s]
    msg.write_int(2)
    msg.write_string("1")
    msg.write_string("2")

    #map [ss]
    msg.write_int(2)
    msg.write_string("1a")
    msg.write_string("1b")
    msg.write_string("2a")
    msg.write_string("2b")

    #map {s[s]}
    msg.write_int(2)

    msg.write_string("1")
    msg.write_int(2)
    msg.write_string("1a")
    msg.write_string("1b")

    msg.write_string("2")
    msg.write_int(2)
    msg.write_string("2a")
    msg.write_string("2b")

    result = (True, 'c', 42, 42.0, 42.0, 'str', ['1', '2'],
                    {'1a': '1b', '2a': '2b'},
                    {'1': ['1a', '1b'], '2': ['2a', '2b']})
    ret = qi.message.message_to_python("bcifds[s]{ss}{s[s]}", msg)
    print "bim"
    print "ret   :", ret
    print "result:", result
    assert(ret == result)

    msg2 = qi.Message()
    qi.message.python_to_message("bcifds[s]{ss}{s[s]}", msg2, *ret)
    print "msg2"
    ret2 = qi.message.message_to_python("bcifds[s]{ss}{s[s]}", msg2)
    print "ret2  :", ret
    print "result:", result
    assert(ret2 == result)




if __name__ == "__main__":
    #test_message()
    test_message2()