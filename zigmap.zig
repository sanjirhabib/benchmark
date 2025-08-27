const std = @import("std");

pub fn main() !void {
    var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
    defer arena.deinit();
    const allocator = arena.allocator();

    var map = std.StringHashMap([]const u8).init(allocator);
    defer map.deinit();

    for (0..5_000_000) |i| {
        const key = try std.fmt.allocPrint(allocator, "id.{d}", .{i % 500_000});
        const val = try std.fmt.allocPrint(allocator, "val.{d}", .{i});
        try map.put(key, val);
    }

    try std.io.getStdOut().writer().print("{d}\n", .{map.count()});
    try std.io.getStdOut().writer().print("{s}\n", .{map.get("id.10000").?});
}

