import java.util.HashMap;
import java.util.Map;

public class Javamap
{
  public static void main (String[]args)
  {
    HashMap < String, String > map = new HashMap <> ();
    for (int i = 0; i < 5000000; i++)
      {
	map.put ("id." + (i % 500000), "val." + i);
      }
    System.out.println (map.size ());
    System.out.println (map.get ("id.10000"));
  }

}
