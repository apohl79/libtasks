struct id_name {
       1: i32 id,
       2: string name
}

service test_service {
	id_name lookup()
}
