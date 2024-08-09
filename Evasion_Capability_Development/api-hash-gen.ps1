$APIsToHash = @("CreateThread")

$APIsToHash | % {
    $api = $_
    
    $hash = 0x35
    [int]$i = 0

    $api.ToCharArray() | % {
        $l = $_
        $c = [int64]$l
        $c = '0x{0:x}' -f $c
        $hash += $hash * 0xab10f29f + $c -band 0xffffff
        $hashHex = '0x{0:x}' -f $hash
        $i++
        write-host "Iteration $i : $l : $c : $hashHex"
    }
    write-host "$api`t $('0x00{0:x}' -f $hash)"
}
