use std::collections::HashMap;
fn main(){
    let mut ret = HashMap::new();
    for i in 0..5000000 {
        ret.insert("id.".to_owned()+&(i%500000).to_string(), "val.".to_owned()+&i.to_string());
        //ret.insert(format!("id.{}", i%500000), format!("val.{}",i));
    }
    println!("{}", ret.len());
    match ret.get("id.10000") {
        Some(val) => println!("{}", val),
        None => println!("ERROR")
    }
}
