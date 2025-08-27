# Create an empty named list (similar to C++ map)
ret <- list()

# Loop to populate the list
for(i in 0:4999999) {
  key <- paste0("id.", i %% 500000)
  value <- paste0("val.", i)
  ret[[key]] <- value
}

# Print the size of the list
cat(length(ret), "\n")

# Print the value for key "id.10000"
cat(ret[["id.10000"]], "\n")
