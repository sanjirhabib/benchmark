var map: [String: String] = [:]
for i in 0...5000000 {
	map["id."+String(i%500000)]="val."+String(i)
}
print(String(map.count))
print(map["id.10000"] ?? ""+"\n")
