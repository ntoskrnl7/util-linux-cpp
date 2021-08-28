#include <gtest/gtest.h>

#include <iostream>
#include <util_linux/lsblk.hpp>

TEST(lsblk_test, lsblk) {
  EXPECT_NO_THROW(auto lsblk = util_linux::lsblk();
                  std::cout << lsblk.dump(4););
}

TEST(lsblk_test, lsblk_sda) {
  EXPECT_NO_THROW(auto sda = util_linux::lsblk("/dev/sda");
                  std::cout << sda.dump(4); EXPECT_EQ(sda["path"], "/dev/sda");
                  EXPECT_EQ(sda["name"], "sda"););
}

TEST(lsblk_test, lsblk_sda1) {
  EXPECT_NO_THROW(
      auto sda1 = util_linux::lsblk("/dev/sda1"); std::cout << sda1.dump(4);
      EXPECT_EQ(sda1["path"], "/dev/sda1"); EXPECT_EQ(sda1["name"], "sda1"););
}