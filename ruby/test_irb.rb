#!./ruby

input = :continue
line_count = 0

while input == :continue
  line_count += 1
  print "rxi(main):#{line_count}:0> "
  input = gets
  if input.nil?
    puts
  elsif input.length > 0
    input.strip!
    puts "=> " + eval(input).inspect
    input = :continue
  end
end

