# Scrypt

## Usage

    go get github.com/steakknife/scrypt

## Pseudoexamples

### Self-calibrating example (adapts to most future hardware/software advances)

```golang
import "github.com/steakknife/scrypt"
import "fmt"

func main() {
        // dont do this exactly, it's an example
        secret, N, r, p, err := scrypt.KeyCalibrated[]byte("password"), []byte("salt"), 16) // 16 bytes returned
        
        if err != nil {
          panic(err)
        }
        fmt.Printf("secret=%#v, N=%d, r=%d, p=%d", secret, N, r, p)
}
```

### Another example (fixed calibration, not future-proof)

```golang
// ...
// Again, don't do this exactly 

// N, r, p := ...

secret2, err := scrypt.Key([]byte("god"), []byte("magic"), N, r, p, 32) // 32 bytes returned

// ...
```

## Acknowledgements

Colin Percival and Tarsnap (I'd use it in a heartbeat)

Go Crypto library

## Patches welcome!
