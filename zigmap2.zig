const std = @import("std");

pub fn main() !void {
    // Initialize an allocator (GeneralPurposeAllocator for safety)
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // Create a StringHashMap to store string-string key-value pairs
    var ret = std.StringHashMap([]const u8).init(allocator);
    defer {
        // Free all keys and values, then the map itself
        var it = ret.iterator();
        while (it.next()) |entry| {
            allocator.free(entry.key_ptr.*);
            allocator.free(entry.value_ptr.*);
        }
        ret.deinit();
    }

    // Loop to populate the map
    var i: i32 = 0;
    while (i < 5_000_000) : (i += 1) {
        // Format key as "id.<i % 500000>"
        const key = try std.fmt.allocPrint(allocator, "id.{}", .{try std.math.mod(i32, i, 500_000)});
        // Format value as "val.<i>"
        const value = try std.fmt.allocPrint(allocator, "val.{}", .{i});

        // Insert into map (put takes ownership of key/value)
        try ret.put(key, value);
    }

    // Print map size
    const stdout = std.io.getStdOut().writer();
    try stdout.print("{}\n", .{ret.count()});

    // Print value for key "id.10000"
    if (ret.get("id.10000")) |value| {
        try stdout.print("{s}\n", .{value});
    } else {
        try stdout.print("Key not found\n", .{});
    }
}
