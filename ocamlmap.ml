let () =
  let ret = Hashtbl.create 500000 in
  for i = 0 to 4999999 do
    Hashtbl.replace ret ("id." ^ string_of_int (i mod 500000)) ("val." ^ string_of_int i)
  done;
  Printf.printf "%d\n" (Hashtbl.length ret);
  Printf.printf "%s\n" (Option.get (Hashtbl.find_opt ret "id.10000"))
