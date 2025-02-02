/*
 * Copyright (c) 2010-2022 Belledonne Communications SARL.
 *
 * This file is part of Liblinphone
 * (see https://gitlab.linphone.org/BC/public/liblinphone).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _L_FILE_CONTENT_H_
#define _L_FILE_CONTENT_H_

#include "content.h"

// =============================================================================

LINPHONE_BEGIN_NAMESPACE

class FileContentPrivate;

class LINPHONE_PUBLIC FileContent : public Content {
public:
	FileContent();
	FileContent(const FileContent &other);
	FileContent(FileContent &&other);

	FileContent *clone() const override {
		return new FileContent(*this);
	}

	FileContent &operator=(const FileContent &other);
	FileContent &operator=(FileContent &&other);

	bool operator==(const FileContent &other) const;

	void setFileSize(size_t size);
	size_t getFileSize() const;

	void setFileName(const std::string &name); // App Locale
	const std::string &getFileName() const;

	void setFileNameSys(const std::string &name); // System Locale
	std::string getFileNameSys() const;

	void setFileNameUtf8(const std::string &name); // UTF8
	std::string getFileNameUtf8() const;

	void setFilePath(const std::string &path); // App Locale
	const std::string &getFilePath() const;

	void setFilePathSys(const std::string &path); // System Locale
	std::string getFilePathSys() const;

	void setFilePathUtf8(const std::string &path); // UTF8
	std::string getFilePathUtf8() const;

	void setFileDuration(int durationInSeconds);
	int getFileDuration() const;

	bool isFile() const override;
	bool isFileTransfer() const override;

	bool isEncrypted() const;
	/**
	 * Return the path to a file's plain version.
	 * This shall be a temporary copy
	 * Caller is then responsible to delete it when no more needed
	 */
	const std::string exportPlainFile() const;

private:
	L_DECLARE_PRIVATE(FileContent);
};

LINPHONE_END_NAMESPACE

#endif // ifndef _L_FILE_CONTENT_H_
