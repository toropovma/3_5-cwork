#include "database.h"

std::ostream& operator<<(std::ostream& ostream, Database::Status status)
{
    switch (status)
    {
    case Database::Status::Ready: return ostream << "Ready";
    case Database::Status::Restricted: return ostream << "Restricted";
    case Database::Status::Unavailable: return ostream << "Unavailable";
    default: return ostream << "Unknown status (" << (int)status << ")";
    }
}

Database::Status Database::GetStatus() const
{
    return m_status;
}

void Database::SetStatus(Status status)
{
    if (m_status == status) return;

    DB_INFO("Database status changed from '{}' to '{}'", m_status, status);
    m_status = status;
}
