//===-- Unittests for fchdir ----------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "src/fcntl/open.h"
#include "src/unistd/close.h"
#include "src/unistd/fchdir.h"
#include "test/UnitTest/ErrnoCheckingTest.h"
#include "test/UnitTest/ErrnoSetterMatcher.h"
#include "test/UnitTest/Test.h"

#include "hdr/fcntl_macros.h"

using LlvmLibcChdirTest = LIBC_NAMESPACE::testing::ErrnoCheckingTest;

TEST_F(LlvmLibcChdirTest, ChangeAndOpen) {
  // The idea of this test is that we will first open an existing test file
  // without changing the directory to make sure it exists. Next, we change
  // directory and open the same file to make sure that the "fchdir" operation
  // succeeded.
  using LIBC_NAMESPACE::testing::ErrnoSetterMatcher::Succeeds;
  constexpr const char *FILENAME = "testdata";
  auto TEST_DIR = libc_make_test_file_path(FILENAME);
  constexpr const char *FILENAME2 = "testdata/fchdir.test";
  auto TEST_FILE = libc_make_test_file_path(FILENAME2);
  constexpr const char *FILENAME3 = "fchdir.test";
  auto TEST_FILE_BASE = libc_make_test_file_path(FILENAME3);

  int dir_fd = LIBC_NAMESPACE::open(TEST_DIR, O_DIRECTORY);
  ASSERT_GT(dir_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  int file_fd = LIBC_NAMESPACE::open(TEST_FILE, O_PATH);
  ASSERT_GT(file_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_THAT(LIBC_NAMESPACE::close(file_fd), Succeeds(0));

  ASSERT_THAT(LIBC_NAMESPACE::fchdir(dir_fd), Succeeds(0));
  file_fd = LIBC_NAMESPACE::open(TEST_FILE_BASE, O_PATH);
  ASSERT_GT(file_fd, 0);
  ASSERT_ERRNO_SUCCESS();
  ASSERT_THAT(LIBC_NAMESPACE::close(file_fd), Succeeds(0));
  ASSERT_THAT(LIBC_NAMESPACE::close(dir_fd), Succeeds(0));
}

TEST_F(LlvmLibcChdirTest, ChangeToNonExistentDir) {
  ASSERT_EQ(LIBC_NAMESPACE::fchdir(0), -1);
  ASSERT_ERRNO_FAILURE();
}
