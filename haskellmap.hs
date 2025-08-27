import qualified Data.Map as Map

main :: IO ()
main = do
    let numIterations = 5000000
    let map = foldl (\m i -> Map.insert ("id." ++ show (i `mod` 500000)) ("val." ++ show i) m) Map.empty [0..numIterations-1]
    print (Map.size map)
    print (map Map.! "id.10000")
