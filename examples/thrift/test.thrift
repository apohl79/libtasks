struct test_msg {
       1: i32 msgid,
       2: string msg
}

service test_service {
	void output(1: test_msg m)
}
