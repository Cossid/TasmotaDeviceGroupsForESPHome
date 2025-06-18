$content = Get-Content 'components/device_groups/device_groups.cpp' -Raw
$pattern = '      #if defined\(USE_ESP_IDF\)\s*\n      if \(!strncmp\(\(char \*\)packet\.payload, kDeviceGroupMessage, sizeof\(DEVICE_GROUP_MESSAGE\) - 1\)\) \{.*?#endif'
$replacement = "#if defined(USE_ESP_IDF)`n      if (!strncmp((char *)packet.payload, kDeviceGroupMessage, sizeof(DEVICE_GROUP_MESSAGE) - 1)) {`n#else`n      if (!strncmp_P((char *)packet.payload, kDeviceGroupMessage, sizeof(DEVICE_GROUP_MESSAGE) - 1)) {`n#endif"
$content = $content -replace $pattern, $replacement
Set-Content 'components/device_groups/device_groups.cpp' -Value $content
Write-Host "Fixed duplicate preprocessor directives" 