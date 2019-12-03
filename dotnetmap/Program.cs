using System;
using System.Collections.Generic;

class Program
{
    static void Main()
    {
        // Create Dictionary.
        Dictionary<string, string> map = new Dictionary<string, string>();

		for(int i=0; i<5000000; i++){
        	map["id."+(i%500000)]="val."+i;
		}
        Console.WriteLine(map.Count);
        Console.WriteLine(map["id.10000"]);
    }
}
