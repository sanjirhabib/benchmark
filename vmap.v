mut m := map[string]string{}
for i in 0 .. 5000000 {
	m['key.'+(i%500000).str()] = 'val.'+i.str()
}
println(m.len)
println(m['key.10000'])

