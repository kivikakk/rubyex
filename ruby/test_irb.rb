#!./ruby

input = :continue
line_count = 0

while input == :continue
  line_count += 1
  print "rxi(main):#{line_count}:0> "
  input = gets
  if input.nil?
    puts
  else
    input.strip!
    if input.length > 0
      puts "=> " + eval(input).inspect
    end
    input = :continue
  end
end

