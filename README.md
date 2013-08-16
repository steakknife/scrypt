# Scrypt

## Usage

    go get github.com/steakknife/scrypt

```golang
import "github.com/steakknife/scrypt"

// dont do this, it's an example
secret := scrypt.KeyCalibrated("password", "salt", 16) // 16 bytes returned

// N := ...
// r := ...
// p := ...
secret2 := scrypt.Key("god", "magic", N, r, p, 32) // 32 bytes returned
```

### Acknowledgements

Colin Percival

Go Crypto library

### Patches welcome!
