package scrypt

// highly-recommend using 
//
//    import "github.com/steakknife/securecompare"
//    
//    securecopmare.Equal(hash, expectedhash) for comparing results

// #include "scrypt_calibrate.h"
// #include "crypto_scrypt.h"
import "C"
import "errors"

const (
    DefaultMaxMem uint64 = 4*1024*1024 // 4 MiB
    DefaultMaxMemFrac float32 = 0.2       // 20% 
    DefaultMaxTime float32 = 0.08      // 80 ms
    maxUInt32 = 1<<32 - 1
    maxRP = 1<<30
)

var (
    MaxMem = DefaultMaxMem // bytes
    MaxMemFrac = DefaultMaxMemFrac // % of memory
    MaxTime = DefaultMaxTime // seconds
    calibrated = false
    calibrated_n C.uint64_t
    calibrated_r C.uint32_t
    calibrated_p C.uint32_t
)

// is scrypt calibrated?
func Calibrated() bool {
    return calibrated
}

// used to automatically pick N, r, p for KeyCalibrated
func Calibrate() error {
    if C.calibrate(C.size_t(MaxMem), C.double(MaxMemFrac), C.double(MaxTime),
                    &calibrated_n, &calibrated_r, &calibrated_p) != 0 {
        calibrated = false
        return errors.New("scrypt calibration error")
    }
    calibrated = true
    return nil
}

// automatically calls Calibrate
// keyLen is the result length, in bytess
func KeyCalibrated(password, salt []byte, keyLen uint) (result []byte, N uint64, r, p uint32, err error) {
    if ! calibrated {
        err = Calibrate()
        if err != nil {
            return
        }
    }
    N, r, p = uint64(calibrated_n), uint32(calibrated_r), uint32(calibrated_p)
    result, err = Key(password, salt, N, r, p, keyLen)
}

var zero = []byte{0}

func addr(buf []byte) *C.uint8_t {
    if len(buf) == 0 {
        buf = zero
    }
    return (*C.uint8_t)(&buf[0])
}

func size(buf []byte) C.size_t {
    return C.size_t(len(buf))
}

// uncalibrated version
// keyLen is the result length, in bytess
func Key(password, salt []byte, N uint64, r, p uint32, keyLen uint) (result []byte, err error) {
    if N <= 1 || N&(N-1) != 0 {
        err = errors.New("scrypt: N must be > 1 and a power of 2")
        return
    }
    if uint64(r)*uint64(p) >= maxRP || r > maxUint32/128/p || r > maxUint32/256 || N > maxUint32/128/r {
        err = errors.New("scrypt: parameters are too large")
        return
    }
    if keyLen <= 0 {
        err = errors.New("scrypt invalid keyLen")
        return
    }
    result = make([]byte, keyLen)
    for i := range result {
        result[i] = 0x00
    }

    buf := (*C.uint8_t)(&result[0])
    buflen := C.size_t(keyLen)

    if C.crypto_scrypt(addr(password), size(password), addr(salt), size(salt),
                       C.uint64_t(N), C.uint32_t(r), C.uint32_t(p),
                       buf, buflen) != 0 {
        err = errors.New("scrypt error")
        return
    }

    return
}
