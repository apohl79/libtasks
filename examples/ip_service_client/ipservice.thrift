struct id_name_type {
  1: i64 id,
  2: string name,
}
 
struct key_value_type {
  1: id_name_type key,
  2: list<id_name_type> values,
}
 
struct response_type {
  1: list<key_value_type> key_values,
}
 
struct ipv6_type {
  1: i64 high,
  2: i64 low,
}
 
service IpService {
  response_type lookup(1: i32 ipv4, 2: ipv6_type ipv6)
}
