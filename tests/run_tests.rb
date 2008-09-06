
Dir["test_*.rb"].each do |test|
  test_name = test[5...-3]
  print "`#{test_name}':\t"
  mri_output = `ruby #{test}`
  rx_output = `../ruby/ruby #{test}`
  puts mri_output == rx_output ? "success" : "failure!"
end

