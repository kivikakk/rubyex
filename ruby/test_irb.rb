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
      begin
	puts "=> " + eval(input).inspect
      rescue Exception => e
	puts "#{e.class}: #{e.message}"
      end
    end
    input = :continue
  end
end

